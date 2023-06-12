#pragma once
#include "../auxs/sfx_contents.h"
namespace studia
{
    struct contents: sfx::dirtree
    {
        contents ()
        {
            sfx::dirtree::contents.pretty = [](record r)
            {
                str name = r.name.from(3);
                if (name.ends_with(".txt"))
                    name.truncate(),
                    name.truncate(),
                    name.truncate(),
                    name.truncate();

                if (name.starts_with("''")
                and name.  ends_with("''")) {
                    name.truncate(); name.erase(0);
                    name.truncate(); name.erase(0);
                    name = extracolor(
                    name); }

                return r.file ? name :
                    bold(name);
            };
        }
    };
}