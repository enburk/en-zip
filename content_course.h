#pragma once
#include "content_topic.h"
namespace content::out
{
    struct course
    {
        unit root;
        array<entry> entries;
        array<str> errors;
        array<str> anomal;

        course (path dir)
        {
            root.units = scan(dir);
            root.sort();
        }
        array<unit> scan (path dir, int level = 0, str parent = "")
        {
            array<unit> units;
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

                unit unit;
                unit.name = un_msdos(fn.from(3));
                unit.order = std::stoi(str(fn.upto(2)));
                unit.path = parent == "" ?
                unit.name : parent + "/" +
                unit.name;

                if (is_directory(path))
                {
                    unit.units = scan(path, level+1, unit.path);
                    units += std::move(unit);
                    continue;
                }

                array<str> lines =
                sys::in::text(path).value();

                content::topic topic{path, lines};
                
                array<str> Lines =
                topic.formatted();

                if (Lines != lines)
                sys::out::write(path,
                    Lines);

                str header = unit.path;
                header.replace_all("/", gray("/"));

                if (not topic.errors.empty()) {
                errors += dark(bold(header));
                errors += topic.errors;
                errors += ""; }

                if (not topic.anomal.empty()) {
                anomal += dark(bold(header));
                anomal += topic.anomal;
                anomal += ""; }

                for (auto chain:
                topic.chains(entries))
                unit.units += chain;

                units += std::move(unit);
            }

            return units;
        }
    };
}
namespace content::in
{
    struct where
    {
    };

    struct course
    {
        unit root;
        array<entry> entries;
        where where;

        void load (path dir)
        {
        }
    };
}
