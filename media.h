#pragma once
#include "eng.h"
#include "../ae/library/cpp/aux_utils.h"
#include "../ae/library/cpp/pix_process.h"
#include "../ae/library/cpp/proto-studio/gui_widget_text_console.h"
namespace media
{
    using std::byte;
    using std::filesystem::path;
    using aux::expected;

    struct resource
    {
        path path;
        str title, comment, license, credit, id;
        array<str> entries;
        array<str> options;
        array<str> keywords;
        bool used = false;
    };

    struct resources
    {
        array<resource> audio;
        array<resource> video;
        void operator += (resources r) {
            audio += r.audio;
            video += r.video;
        }
    };

    namespace report
    {
        gui::text::console * out = nullptr;
        gui::text::console * err = nullptr;
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

    resources scan (path dir, resource common = resource{})
    {
        resources resources;

        std::map<path, bool> identified;

        array<str> video = {".jpg", ".jpeg", ".png"};
        array<str> audio = {".mp3", ".ogg"};

        *report::out << "scan " + dir.string();
        for (std::filesystem::directory_iterator
            next(dir), end; next != end; ++next)
        {
            path entry = next->path();
            str name = entry.filename().string();
            if (name.starts_with(".")) continue;
            name = un_msdos(name);

            str title, meta, ignore;
            name.split_by("{{", title, meta); title.strip();
            meta.split_by("}}", meta, ignore); meta.strip();

            resource resource = common;
            if (is_directory(entry) && meta != "")
                resource.id = "{{" + to_msdos(meta) + "}}" +
                resource.id;

            str credit, license;
            meta.split_by("$", credit,  meta); credit .strip();
            meta.split_by("#", license, meta); license.strip();
            if (credit  != "") resource.credit  = credit;
            if (license != "") resource.license = license;
            
            array<str> options = meta.split_by("#");
            for (str & option : options) option.strip();

            if (is_directory(entry))
            {
                resources += scan(entry, resource);
            }
            if (is_regular_file(entry))
            {
                if (title != "") resource.title = title;

                resource.id = entry.filename().string();
                if (common.id != "")
                    resource.id += " " + common.id;
                report::id2path[resource.id] += entry;

                *report::out << "scan " + entry.string();
                str ext = entry.extension().string();
                ext = ext.ascii_lowercased();

                if (!audio.contains(ext) &&
                    !video.contains(ext))
                {
                    identified[entry] |= false;
                    continue;
                }

                path txt = entry;
                txt.replace_extension(".txt");
                if (std::filesystem::exists(txt))
                {
                    identified[txt] = true;

                    std::ifstream stream(txt); str text = std::string{(
                    std::istreambuf_iterator<char>(stream)),
                    std::istreambuf_iterator<char>()};

                    if (text.starts_with("\xEF" "\xBB" "\xBF"))
                        text.upto(3).erase(); // UTF-8 BOM

                    array<str> title_lines;
                    array<str> lines = text.split_by("\n");
                    for (str line : lines)
                    {
                        line.strip();

                        if (line.starts_with("#")) {
                            resource.options += line.from(1);
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

                if (audio.contains(ext)) resources.audio += resource; else
                if (video.contains(ext)) resources.video += resource;
            }
        }

        for (auto [path, ok] : identified)
            if (!ok) report::unidentified += path;

        return resources;
    }
}