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
        int entry = -1;
        array<unit> units;
        unit* parent = nullptr;
        enum Kind {theme, topic, chain, leaf} kind = leaf;

        void init (int number_of_entries)
        {
            if (entry >= number_of_entries)
            throw std::runtime_error("content::unit"
                " entry=" + std::to_string(entry) +
                " entries=" + std::to_string(number_of_entries) +
                " \"" + path + "\"");

            for (auto& unit: units)
            {
                unit.parent = this;
                unit.init(number_of_entries);
                kind = min(kind, upper(
                unit.kind));
            }
        }

        Kind upper (Kind k) {
            return k == leaf?
            chain: k == chain?
            topic:
            theme;
        }

        //unit* first (Kind k)
        //{
        //}
        //
        //unit* last (Kind k)
        //{
        //}
        //
        //unit* prev (Kind k)
        //{
        //}
        //
        //unit* next (Kind k)
        //{
        //}

        void sort ()
        {
            std::ranges::sort(units, [](auto& a, auto& b)
            {
                if (a.order < b.order) return true;
                if (b.order < a.order) return false;
                return a.name < b.name;
            });

            for (auto& Unit: units)
            for (auto& unit: Unit.units)
                unit.parent =
                &Unit;

            for (auto& unit: units)
                unit.sort();
        }

        void shuffle()
        {
            auto i = units.begin();
            auto j = units.begin();
            auto e = units.end();

            while (i != e)
            {
                while (j != e and
                    i->order ==
                    j->order)
                    j++;

                int n = (int)(j - i);
                for (int nn=n; --nn;)
                {
                    int m;
                    m = aux::random(0, n-1);
                    std::rotate(i, i+m, j);
                    m = aux::random(0, n-1);
                    std::rotate(i, i+m, j);
                }

                i = j;
            }

            for (auto& Unit: units)
            for (auto& unit: Unit.units)
                unit.parent =
                &Unit;

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