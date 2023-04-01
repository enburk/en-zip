#pragma once
#include <stack>
#include "media.h"
#include "eng_parser.h"
#include "eng_phenomena.h"
namespace media::scan
{
    bool parse_links (str& title, str& links)
    {
        int pos = 0;  std::stack<int> lefts; 

        while (pos < title.size())
        {
            if (title[pos] == '{'){ lefts.push(pos); title.erase(pos); } else
            if (title[pos] == '}')
            {
                if (lefts.size() <= 0) return false;
                str link = title.substr(lefts.top(), pos-lefts.top());
                lefts.pop(); title.erase(pos);
                if (link != "")
                {
                    str S, s; bool capital = false;
                    for (str c: aux::unicode::glyphs(link))
                    {
                        if (c == "_") { capital = true; continue; }
                        S += c; if (not capital) c = eng::lowercased(c);
                        s += c; capital = false;
                    }
                    if (links != "")
                    links += "][";
                    links += S;
                    if (s != S)
                    links += "][",
                    links += s;
                }
            }
            else pos++;
        }

        return lefts.size() == 0;
    }

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
            str name = is_directory(path) ?
                path.filename().string():
                path.stem().string();

            if (name.starts_with(".")) continue; else
                name = un_msdos(name);

            if (true) {
            if (is_directory(path))
            dataelog << ">>> " + dir.string() + "/";
            dataelog << path.filename().string();
            dataelog << "\n"; }

            resource resource = common;

            auto check = [path](str name) {
                if (name == "") return false;
                if (not name.contains("}}"))
                logs::err << red(bold(
                "no matching }}: " +
                 path.string()));
                return true; };

            name.strip();
            str yadda = name.extract_from("###");
            str title = name.extract_upto("{{"); bool meta_present = check(name);
            str meta  = name.extract_upto("}}");
            yadda = name;

            if (meta_present) // even if empty
            {
                str credit, license;
                meta.split_by("$", credit, license);
                license.strip();
                credit.strip();
                
                if (credit.starts_with("NASA") or credit.starts_with("ESA") or
                    credit.starts_with("NOAA") or credit.starts_with("ESO") or
                    credit.starts_with("NNSA") or credit.starts_with("USN") or
                    credit.starts_with("USDA") or credit.starts_with("DOE") or
                    credit.starts_with("USDE") or credit.starts_with("NIH") or
                    credit.starts_with("USGS") or
                    credit.starts_with("USAF"))
                    credit = "Credit: " +
                    credit;

                if (license.starts_with("dreamstime")) credit += "/Dreamstime";
                if (license.starts_with("wiki"      )) credit += "/Wikimedia";
                if (license.starts_with("cc-by-sa"  )) credit += " CC-BY-SA"; else
                if (license.starts_with("cc-by"     )) credit += " CC-BY";

                resource.credit = credit;
            }

            str optio = title.extract_from("##");
            str links = title.extract_from("[" ); links.strip("[]");
            str commt = title.extract_from("%%");
            str sense = title.extract_from("@" );

            if (yadda.starts_with("## "))
                optio += yadda;

            if (not parse_links(title, links))
                err << red(bold(
                "parse links error: " +
                 path.string()));

            resource.title = title;
            resource.sense = sense;
            resource.comment = commt;
            resource.abstract = title;
            resource.entries += links.split_strip_by("][");
            resource.options += optio.split_strip_by("##");
            resource.entries.erase_all("");
            resource.options.erase_all("");

            // voice-bunny processing legacy
            std::erase_if(resource.entries,
            [](auto s){ return s == "="; });

            if (is_directory(path))
            {
                if (meta != "")
                resource.id += " {{" + to_msdos(meta) + "}}";

                auto credit = path / "!credit.txt";
                if (std::filesystem::exists(credit)) {
                resource.credit = str(sys::in::text_lines(credit), "<br>");
                identified[credit] = true; }

                scan(path, resources, level+1, resource);
            }
            else if (is_regular_file(path))
            {
                resource.path = path;
                resource.id = path.stem().string() +
                resource.id + path.extension().string();

                str ext = str(path.extension().string()).ascii_lowercased();
                if (ext == ".uid-zps") continue; // Zone Studio pix edits
                if (false) out << "scan " + path.string();

                if (audioexts.contains(ext)) resource.kind = "audio"; else
                if (videoexts.contains(ext)) resource.kind = "video"; else
                {
                    if (path.filename() != "!credit.txt")
                    identified[path] |= false;
                    continue;
                }

                auto txt = path;
                txt.replace_extension(".txt");
                if (std::filesystem::exists(txt))
                {
                    identified[txt] = true;

                    bool title_stop = false;
                    array<str> title_lines;
                    array<str> comment_lines;
                    for (str line: sys::in::text_lines(txt))
                    {
                        line.strip();

                        if (line.starts_with("###"))
                            break;

                        if (not aux::unicode::ok(line))
                        err << red(txt.string() +
                        " !UTF-8: [" + line + "]");

                        if (line.starts_with("**")) {
                            title_stop = true; str
                            comment = line.from(2);
                            comment.strip();
                            comment_lines +=
                            comment;
                        }
                        else
                        if (line.starts_with("##")) {
                            title_stop = true;
                            str option = line.from(2); option.strip();
                            if (option == "") continue;
                            if (option.starts_with("credit ")) {
                                resource.credit = option.from(7);
                                resource.credit.strip();
                                continue;
                            }
                            resource.options += option;
                        }
                        else
                        if (line.starts_with("[[")) { line.strip("[]");
                            resource.keywords += line.split_by("]][[");
                            title_stop = true;
                        }
                        else
                        if (line.starts_with("[")) { line.strip("[]");
                            resource.entries += line.split_by("][");
                            title_stop = true;
                        }
                        else
                        if (not title_stop)
                            title_lines += line;
                    }

                    while (title_lines.size() > 0 &&
                           title_lines.back() == "")
                           title_lines.truncate();

                    if (title_lines.size() > 0)
                    {
                        title = str(title_lines,
                        title_lines.front().
                        starts_with("<") ?
                        "" : "<br>");

                        str links = "";
                        if (not parse_links(title, links))
                            err << red(bold(
                            "parse links error: " +
                             path.string()));

                        resource.title = title;
                        resource.entries +=
                        links.split_by("][");
                    }

                    if (comment_lines.size() > 0)
                        resource.comment = str(
                        comment_lines, "<br>");
                }

                report::id2path[resource.id] += path; // check for same id

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