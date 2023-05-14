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

        struct search_entry
        {
            str word, entry, link;

            friend void operator >> (sys::in::pool& pool, search_entry& x) {
                pool >> x.word;
                pool >> x.entry;
                pool >> x.link;
            }
            friend void operator << (sys::out::pool& pool, search_entry const& x) {
                pool << x.word;
                pool << x.entry;
                pool << x.link;
            }
        };
        array<search_entry> searchmap;

        course (path dir)
        {
            root.units = scan(dir);
            root.sort();

            std::ranges::
            stable_sort(
            searchmap, {}, &
            search_entry::
            word);
        }
        array<unit> scan (path dir, int level = 0, str parent = "")
        {
            array<unit> units;
            using logs::out;
            using logs::err;

            if (level < 3)
            out << "scan " + str(dir);
            for (std::filesystem::directory_iterator
                next(dir), end; next != end; ++next)
            {
                path path = next->path();
                str fn = is_directory(path) ?
                str(path.filename()):
                str(path.stem());

                if (fn.starts_with("."))
                    continue;

                if (fn.size() < 4
                or  fn [2] != ' '
                or  not str(fn[0]).digital()
                or  not str(fn[1]).digital()
                or  is_regular_file(path)
                and path.extension() != ".txt") {
                    err << red(str(path));
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
                sys::in::text_lines(path);

                topic topic{path, lines};
                
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

                for (auto& e: topic.entries)
                for (str w: e.vocabulary)
                searchmap += search_entry(
                    eng::asciized(
                    w.extract_upto("@")).
                    ascii_lowercased(),
                    e.eng, e.link);

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
    struct course
    {
        unit root;
        array<entry> entries;

        void load (path p1, path p2)
        {
            if (true) sys::in::file(p1) >> root;
            if (true) sys::in::file(p2) >> entries;

            root.init(entries.size());
        }

        unit* find (str path, unit* unit = nullptr)
        {
            if (!unit) unit = &root;

            str name = path.extract_upto("/");

            auto it = unit->units.find_if(
                [name](auto& u){ return
                    u.name == name; });

            if (it == unit->units.end())
                return unit;

            unit = &*it;
            return path == "" ? unit :
                find(path, unit);
        }
    };
}
