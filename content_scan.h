#pragma once
#include "content_topic.h"
namespace content
{
    using std::filesystem::path;

    unit scan (path dir, array<entry>& entries, int level = 0, str name = "")
    {
        unit  unit;
        using logs::out;
        using logs::err;

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
            or  not str(fn[0]).digital()
            or  not str(fn[1]).digital()
            or  is_regular_file(path)
            and path.extension() != ".txt") {
                err << red(path.string());
                continue; }

            unit.title = name;
            unit.order = std::stoi(str(fn.upto(2)));

            fn = fn.from(3);
            fn = un_msdos(fn);
            if (name != "")
            name += "/";
            name += fn;

            if (is_directory(path))
            {
                unit.units += scan(path, entries, level+1, name);
                if (unit.units.back().units.empty()) {
                    unit.units.truncate();
                    out << path.string() +
                    " SKIPPED";
                }
                continue;
            }

            array<str> lines =
            dat::in::text(path).value();

            content::topic topic{lines};
                
            array<str> Lines =
            topic.formatted();

            if (Lines != lines)
            dat::out::write(path,
                Lines);

            for (auto& entry: topic.entries)
            for (auto& error: entry.errors)
            err << red(bold(path.string() + " (" +
            std::to_string(entry.line) + "): " +
            error));

            for (auto chain:
            topic.chains(entries))
            unit.units += chain;
        }

        return unit;
    }
}
