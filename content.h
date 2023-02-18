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
        str name;
        str path;
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

        friend void operator >> (sys::in::pool& pool, unit& x) {
            pool >> x.name;
            pool >> x.path;
            pool >> x.order;
            pool >> x.entry;
            pool >> x.units;
        }
        friend void operator << (sys::out::pool& pool, unit const& x) {
            pool << x.name;
            pool << x.path;
            pool << x.order;
            pool << x.entry;
            pool << x.units;
        }
    };
}