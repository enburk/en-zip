#pragma once
#include "eng_dictionary.h"
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

        array<str> video = {".jpg", ".jpeg", ".png"};
        array<str> audio = {".mp3", ".ogg"};

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

            str title, meta, yadda;
            name.split_by("{{", title, meta); title.strip();
            meta.split_by("}}", meta, yadda); meta.strip();

            str credit, m = meta;
            m.split_by("$", credit, m); credit.strip();
            if (credit != "") resource.credit = credit;
            
            array<str> options = m.split_by("##");
            for (str & option : options) {
                option.strip(); if (option == "") continue;
                resource.options += option;
            }

            if (is_directory(entry))
            {
                if (meta != "")
                    resource.id +=
                    " {{" + to_msdos(meta) + "}}";

                resources += scan(entry, resource);
            }
            else if (is_regular_file(entry))
            {
                resource.path = entry;
                resource.id = entry.stem().string() +
                resource.id + entry.extension().string();

                if (title != "") resource.title = title;

                *report::out << "scan " + entry.string();
                str ext = entry.extension().string();
                ext = ext.ascii_lowercased();

                if (audio.contains(ext)) resource.kind = "audio"; else
                if (video.contains(ext)) resource.kind = "video"; else
                {
                    identified[entry] |= false;
                    continue;
                }

                path txt = entry;
                txt.replace_extension(".txt");
                if (std::filesystem::exists(txt))
                {
                    identified[txt] = true;

                    array<str> title_lines;
                    array<str> lines = dat::in::text(txt).value();
                    for (str line : lines)
                    {
                        line.strip();

                        if (line.starts_with("##")) {
                            str option = line.from(2);
                            option.strip(); if (option == "") continue;
                            resource.options += option;
                        }
                        else
                        if (line.starts_with("[[")) {
                            line.replace_all("] [",   "][");
                            line.replace_all("]  [",  "][");
                            line.replace_all("]   [", "][");
                            line.strip("[]");
                            resource.keywords += line.split_by("]][[");
                        }
                        else
                        if (line.starts_with("[")) {
                            line.replace_all("] [",   "][");
                            line.replace_all("]  [",  "][");
                            line.replace_all("]   [", "][");
                            line.strip("[]");
                            resource.entries += line.split_by("][");
                        }
                        else title_lines += line;
                    }

                    while (title_lines.size() > 0 &&
                           title_lines.back() == "")
                           title_lines.truncate();

                    if (title_lines.size() > 0)
                        resource.title = str(title_lines);
                }

                report::id2path[resource.id] += entry; // check for same id
                resources += resource;
            }
        }

        for (auto [path, ok] : identified)
            if (!ok) report::unidentified += path;

        return resources;
    }
}