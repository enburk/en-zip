#pragma once
#include "content_scan.h"
namespace content::out
{
    struct course
    {
        unit root;
        array<entry> entries;

        course (path dir)
        {
            root = scan(dir, entries);
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
