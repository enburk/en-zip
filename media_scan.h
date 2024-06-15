#pragma once
#include <stack>
#include "eng_parser.h"
#include "eng_phenomena.h"
#include "media_resource.h"
#include "media_resource+.h"
namespace media::scan
{
    std::ofstream dataelog;

    auto scan (path dir, int level = 0, resource common = {}) -> array<resource>
    {
        array<resource> resources;

        std::map<path, bool> identified;

        using logs::out;
        using logs::err;

        aux::timing t0;

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
                    resource.credit = str(sys::
                    text_lines(credit), "<br>"),
                    identified[credit] = true;

                array<media::resource> rr;

                auto cache = path / "resources.txt";
                bool scan_once  = str(path.filename()).contains("scan-once!");
                bool scan_ready = scan_once and std::filesystem::exists(cache);
                bool scan_write = scan_once and not scan_ready;
                bool scan_fresh = not scan_ready;

                if (scan_ready) sys::in::file(cache) >> rr;

                if (scan_fresh) rr = scan(path, level+1, resource);

                if (scan_write) sys::out::file(cache) << rr;

                resources += std::move(rr);
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

                if (resource.credit != "")
                for(str option: resource.options)
                if (option.starts_with("date "))
                    resource.credit += ", " + italic(
                    option.from(5));

                resource.credit.replace_all(", read by", "<br>read by");
                resource.credit.replace_all(", narrated by", "<br>narrated by");

                resources += resource;

                if (resource.title.contains(one_of("#@")))
                {
                    err << red(
                    bold("#@: ")) +
                    str(dir) + "/" +
                    resource.title;
                }
                if  (resource.kind == "audio")
                for (str option: resource.options)
                {
                    static const array<str> upto5 = {"crop ", "date "};
                    static const array<str> exact = {"=", "sic!", "Case", "wide",
                    "us","uk","ca","au","ru", "poem","song","sound","number","pixed",
                    "fade","fade in","fade out",
                    "{1}","{2}","{3}","{1,2}",
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
                    static const array<str> exact = {"=", "sic!", "Case", "wide",
                    "6+","8+","10+","12+","14+","16+","18+","21+","99+",
                    "qropt!", "noqrop"};
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

        aux::timing t1;

        if (level < 2)
        out << "scan " + str(dir) +
        gray(" " + aux::ms(t1-t0) + " ms");

        return resources;
    }
}