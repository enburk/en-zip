#pragma once
#include <execution>
#include "content_table.h"
namespace studio::build
{
    using std::filesystem::path;

    array<content::topic> topics;

    void scan (path dir,
        gui::console & out,
        gui::console & err,
        int level = 0,
        str name = "")
    {
        if (level < 3)
        out << "scan " + dir.string();
        for (std::filesystem::directory_iterator
            next(dir), end; next != end; ++next)
        {
            path path = next->path();
            str fn = is_directory(path) ?
                path.filename().string():
                path.stem().string();

            if (fn.starts_with("."))
                continue;

            if (fn.size() < 4
            or  fn [2] != ' '
            or  is_regular_file(path)
            and path.extension() != ".txt") {
                err << red(path.string());
                continue; }

            fn = fn.from(3);
            fn = un_msdos(fn);
            if (name != "")
            name += "/";
            name += fn;

            if (is_directory(path))
            {
                scan(path, out, err, level+1, name);
            }
            else if (is_regular_file(path))
            {
                if (false)
                out << "scan " + path.string();
                
                array<str> lines =
                dat::in::text(path).value();

                topics += content::topic{lines};
                
                array<str> Lines =
                topics.back().formatted();

                if (Lines != lines)
                dat::out::write(path, Lines);

                for (auto& entry: topics.back().entries)
                for (str option: entry.opt.unknown)
                err << red(bold(path.string() + " (" +
                std::to_string(entry.line) + "): " +
                "UNKNOWN OPTION: " + option));
            }
        }
    }

    bool content_update (gui::console & out, gui::console & err)
    {
        scan("content", out, err);

        return false;
    }
}
