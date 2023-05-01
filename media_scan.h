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
        out << "scan " + str(dir);
        for (std::filesystem::directory_iterator
            next(dir), end; next != end; ++next)
        {
            path path;
            path = next->path();
            if (str(path.filename()).
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
                dataelog << ">>> " + str(dir) + "/";
                dataelog << str(path.filename());
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
                resource.id = str(path.stem()) +
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

                array<str> options;
                for (str o: resource.options) {
                    o = o.extract_upto(" ");
                    if (not options.contains(o))
                        options += o; else
                        logs::err << red(bold(
                        "duplicate option " + o +
                        ": " + str(path)));
                }

                if (resource.credit == "-")
                    resource.credit = "";

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
                    str(dir) + "/" +
                    resource.title +" {" +
                    resource.sense + "}");

                    if (resource.title.contains(one_of("{}")))
                    err << yellow(
                    str(dir) + "/" +
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
                    str(dir) + "/" +
                    resource.title +
                    " OPTION: ["
                    + option +
                    "]");
                }
                if  (resource.kind == "video")
                for (str option: resource.options)
                {
                    static const array<str> upto5 = {"crop ", "qrop ", "date "};
                    static const array<str> exact = {"=", "sic!", "qropt!", "Case", "{links}",
                    "6+","8+","10+","12+","14+","16+","18+","21+","99+"};
                    if (not exact.contains(option)
                    and not upto5.contains(option.upto(5)))
                    err << red(
                    str(dir) + "/" +
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