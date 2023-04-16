#pragma once
#include <stack>
#include "eng_parser.h"
#include "eng_phenomena.h"
#include "media_resource.h"
#include "media_resource+.h"
namespace media::scan
{
    std::ofstream dataelog;

    void scan (path dir, array<resource>& resources, int level = 0, resource common = {})
    {
        std::map<path, bool> identified;

        using logs::out;
        using logs::err;

        if (level < 2)
        out << "scan " + dir.string();
        for (std::filesystem::directory_iterator
            next(dir), end; next != end; ++next)
        {
            path path;
            path = next->path();
            if (path.filename().string().
                starts_with("."))
                continue;

            resource resource(path);
            resource.entries += common.entries;
            resource.options += common.options;
            if (resource.credit == "")
                resource.credit =
                  common.credit;

            if (not resource.options.contains("xlam"))
            {
                if (is_directory(path))
                dataelog << ">>> " + dir.string() + "/";
                dataelog << path.filename().string();
                dataelog << "\n";
            }

            if (is_directory(path))
            {
                resource.id = common.id;
                if (resource.meta != "")
                    resource.id += " {{" + to_msdos(
                    resource.meta) + "}}";

                auto credit = path / "!credit.txt";
                if (std::filesystem::exists(credit))
                    resource.credit = str(sys::in::
                    text_lines(credit), "<br>"),
                    identified[credit] = true;

                scan(path,
                resources, level+1,
                resource);
            }
            else if (is_regular_file(path))
            {
                resource.id = common.id;
                resource.id = path.stem().string() +
                resource.id + path.extension().string();

                str ext = str(path.extension().string()).ascii_lowercased();
                if (ext == ".uid-zps") continue; // Zone Studio pix edits

                if (audioexts.contains(ext)) resource.kind = "audio"; else
                if (videoexts.contains(ext)) resource.kind = "video"; else
                {
                    if (path.filename() != "!credit.txt")
                    identified[path] |= false;
                    continue;
                }

                report::id2path[resource.id] += path; // check for same id

                auto txt = path;
                txt.replace_extension(".txt");
                if (std::filesystem::exists(txt))
                {
                    Resource r(txt);

                    if (r.title   != "") resource.title   = r.title;
                    if (r.comment != "") resource.comment = r.comment;
                    if (r.credit  != "") resource.credit  = r.credit;
                    resource.options += r.options;
                    resource.entries += r.entries;

                    identified[txt] = true;
                }

                resources += resource;

                if (true)
                if (not resource.options.contains("{links}"))
                {
                    static const auto ee =
                    eng::list::sensitive*
                    eng::lexical_items*
                    eng::lexical_notes*
                    eng::related_items*
                    str("1, 2, 3").split_by(", ");
                    if (resource.sense != ""
                    and not ee.contains(resource.sense))
                    err << blue(
                    dir.string() + "/" +
                    resource.title +" {" +
                    resource.sense + "}");

                    if (resource.title.contains(one_of("{}")))
                    err << yellow(
                    dir.string() + "/" +
                    resource.title);
                }
                if  (resource.kind == "audio")
                for (str option: resource.options)
                {
                    static const array<str> upto5 = {"crop ", "date "};
                    static const array<str> exact = {"=", "sic!", "Case", "{links}",
                    "us","uk","ca","au","ru", "poem","song","sound","number","pixed",
                    "fade","fade in","fade out",
                    "reduced","unclear"};
                    if (not exact.contains(option)
                    and not upto5.contains(option.upto(5)))
                    err << red(
                    dir.string() + "/" +
                    resource.title +
                    " OPTION: ["
                    + option +
                    "]");
                }
                if  (resource.kind == "video")
                for (str option: resource.options)
                {
                    static const array<str> upto5 = {"crop ", "qrop ", "date "};
                    static const array<str> exact = {"=", "sic!", "Case", "{links}",
                    "6+","8+","10+","12+","14+","16+","18+","21+","99+"};
                    if (not exact.contains(option)
                    and not upto5.contains(option.upto(5)))
                    err << red(
                    dir.string() + "/" +
                    resource.title +
                    " OPTION: ["
                    + option +
                    "]");
                }
            }
        }

        for (auto [path, ok] : identified)
        if (!ok) report::unidentified += path;
    }
}