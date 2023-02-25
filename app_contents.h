#pragma once
#include "../auxs/sfx_contents.h"
#include "app.h"
namespace app
{
    struct contents:
    widget<contents>
    {
        typedef sfx::contents::record record;
        typedef sfx::contents::Records Records;

        gui::area<sfx::contents> contents;
        gui::binary_property<str>& selected =
            contents.object.selected;

        Records fill(content::unit& parent, int level = 1)
        {
            Records records;

            for (auto& unit: parent.units)
            {
                if (unit.name == "")
                    continue;

                str name = unit.name;
                if (name.starts_with("''")
                and name.  ends_with("''")) {
                    name.truncate(); name.erase(0);
                    name.truncate(); name.erase(0);
                    name = lightblue(name); }

                auto subunits = fill(unit, level+1);

                record r;
                r.path = unit.path;
                r.name = name;
                r.open = subunits.empty();
                r.file = subunits.empty();
                r.level = level;
                records += r;
                records += std::move(subunits);
            }

            return records;
        }

        void reload (str name, content::unit& root)
        {
            contents.object.name = name;
            contents.object.records =
                fill(root);
        }

        void on_change (void* what) override
        {
            if (what == &coord)
                contents.coord =
                coord.now.local();

            if (what == &contents)
                notify();
        }
    };
}
