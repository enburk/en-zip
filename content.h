#pragma once
#include "abc.h"
namespace content
{
    using std::filesystem::path;
    using aux::expected;
    using sys::byte;

    namespace report
    {
        bool updated = false;
    }
    namespace logs
    {
        optional_log out;
        optional_log err;
    }

    struct unit
    {
        str title;
        str fullname;
        int order = 0;
        int entry = 0;
        array<unit> units;

        void sort ()
        {
            std::ranges::sort(units, {}, &unit::order);
            for (auto& unit: units)
                unit.sort();
        }

        void shuffle()
        {
            std::random_device rd;
            std::mt19937 g(rd());
            //std::shuffle(v.begin(), v.end(), g);
            for (auto& unit: units)
                unit.shuffle();
        }
    };
}