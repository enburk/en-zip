#pragma once
#include "eng_parser.h"
#include "../ae/proto-studio/data.h"
#include "../ae/proto-studio/pix_process.h"
#include "../ae/proto-studio/gui_widget_console.h"
namespace media
{
    using sys::byte;
    using std::filesystem::path;
    using data::expected;

    struct resource
    {
        path path;
        str id, kind;
        str title, comment, credit;
        array<str> options;
        array<str> entries;
        array<str> keywords;
    };

    namespace report
    {
        gui::console * out = nullptr;
        gui::console * err = nullptr;
        std::unordered_map<str, array<path>> id2path;
        array<path> unidentified;
        bool data_updated = false;
    }

    str un_msdos (str s)
    {
        s.replace_all("~!",  "?");
        s.replace_all('~',1, "/");
        s.replace_all('.',2, ":");
        return s;
    }
    str to_msdos (str s)
    {
        s.replace_all("?", "~!");
        s.replace_all("/", "~" );
        s.replace_all(":", "..");
        return s;
    }

    array<resource> scan (path dir, resource common = {})
    {
        array<resource> resources;

        std::map<path, bool> identified;

        array<str> video = {".png", ".jpg", ".jpeg" };
        array<str> audio = {".mp3", ".ogg", ".wav"};

        *report::out << "scan " + dir.string();
        for (std::filesystem::directory_iterator
            next(dir), end; next != end; ++next)
        {
            path entry = next->path();
            str name = is_directory(entry) ?
                entry.filename().string():
                entry.stem().string();

            if (name.starts_with(".")) continue;
            name = un_msdos(name);

            resource resource = common;

            str title, meta, yadda; bool meta_present = 
            name.split_by("{{", title, meta); title.strip();
            meta.split_by("}}", meta, yadda); meta.strip();

            str credit, license;
            meta.split_by("$", credit, license); credit.strip();
            if (credit.starts_with("NASA") or credit.starts_with("ESA") or
                credit.starts_with("NOAA") or credit.starts_with("ESO") or
                credit.starts_with("NNSA") or credit.starts_with("USN") or
                credit.starts_with("USDA") or credit.starts_with("DOE") or
                credit.starts_with("USDE") or credit.starts_with("NIH") or
                credit.starts_with("USGS") or
                credit.starts_with("USAF"))
                credit = "Credit: " + credit;
            if (license.starts_with("dreamstime")) credit += "/Dreamstime";
            if (license.starts_with("wiki"      )) credit += "/Wikimedia";
            if (license.starts_with("cc-by-sa"  )) credit += " CC-BY-SA"; else
            if (license.starts_with("cc-by"     )) credit += " CC-BY";
            if (meta_present) resource.credit = credit;
            
            str links, optio;
            title.split_by(" ## ", title, optio);
            title.split_by("[",    title, links);
            links.strip("[ ]");
            title.strip();

            resource.entries += links.split_by("][");
            resource.options += optio.split_by(" ## ");

            if (is_directory(entry))
            {
                if (meta != "")
                    resource.id +=
                    " {{" + to_msdos(meta) + "}}";


                path credit = entry / "!credit.txt";
                if (std::filesystem::exists(credit)) {
                    resource.credit = str(dat::in::text(credit).value(), "<br>");
                    identified[credit] = true;
                }

                resources += scan(entry, resource);
            }
            else if (is_regular_file(entry))
            {
                resource.path = entry;
                resource.id = entry.stem().string() +
                resource.id + entry.extension().string();

                *report::out << "scan " + entry.string();
                str ext = entry.extension().string();
                ext = ext.ascii_lowercased();

                if (audio.contains(ext)) resource.kind = "audio"; else
                if (video.contains(ext)) resource.kind = "video"; else
                if (entry.filename() != "!credit.txt")
                {
                    identified[entry] |= false;
                    continue;
                }

                path txt = entry;
                txt.replace_extension(".txt");
                if (std::filesystem::exists(txt))
                {
                    identified[txt] = true;

                    bool title_stop = false;
                    array<str> title_lines;
                    array<str> comment_lines;
                    array<str> lines = dat::in::text(txt).value();
                    for (str line : lines)
                    {
                        line.strip();

                        if (line.starts_with("**")) {
                            title_stop = true;
                            str comment = line.from(2); comment.strip();
                            comment_lines += comment;
                        }
                        else
                        if (line.starts_with("##")) {
                            title_stop = true;
                            str option = line.from(2); option.strip();
                            if (option == "") continue;
                            if (option.starts_with("credit")) {
                                resource.credit = option.from(7);
                                continue;
                            }
                            resource.options += option;
                        }
                        else
                        if (line.starts_with("[[")) {
                            line.replace_all("] [",   "][");
                            line.replace_all("]  [",  "][");
                            line.replace_all("]   [", "][");
                            line.strip("[]");
                            resource.keywords += line.split_by("]][[");
                            title_stop = true;
                        }
                        else
                        if (line.starts_with("[")) {
                            line.replace_all("] [",   "][");
                            line.replace_all("]  [",  "][");
                            line.replace_all("]   [", "][");
                            line.strip("[]");
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
                        title = str(title_lines);

                    if (comment_lines.size() > 0)
                        resource.comment = str(
                        comment_lines, "<br>");
                }

                if (title != "") resource.title = title;
                if (title != "") resource.entries += title;

                if (false) *report::out << 
                "<font color=#800080>" + resource.title + "</font>" +
                "<font color=#000080>" + "[" + str(resource.entries, "][") + "]" + "</font>" +
                "<font color=#008000>" + "{" + str(resource.options, "}{") + "}" + "</font>" ;

                report::id2path[resource.id] += entry; // check for same id
                resources += resource;
            }
        }

        for (auto [path, ok] : identified)
            if (!ok) report::unidentified += path;

        return resources;
    }
}