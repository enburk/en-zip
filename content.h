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
        enum Kind {theme, topic, chain, leaf} kind = leaf;
        unit* parent = nullptr;
        array<unit> units;

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

        unit& add (Kind Kind, int Order, str Name)
        {
            unit unit;
            unit.kind = Kind;
            unit.name = Name;
            unit.order = Order;
            unit.parent = this;
            unit.path = Name == "" ? "" : path == "" ?
            unit.name : path + "/" + unit.name;
            units += unit; return
            units.back();
        }
        unit& add_theme (int Order, str Name) { return add(theme, Order, Name); }
        unit& add_topic (int Order, str Name) { return add(topic, Order, Name); }
        unit& add_chain (int Order) { return add(chain, Order, ""); }
        unit& add_leaf  (int Order) { return add(leaf , Order, ""); }

        unit* first_topic ()
        {
            for (unit& u: units)
            if (u.kind == topic)
            return &u;
            return nullptr;
        }

        unit* first_theme ()
        {
            for (unit& u: units)
            if (u.kind == theme)
            return u.first_theme(); else
            return this;
            return this;
        }

        unit* next_theme (int current)
        {
            for (unit& u: units.from(current))
            if (u.kind == theme)
            return u.first_theme();

            if (not parent) return nullptr;

            for (int n=0; n < parent->units.size(); n++)
            if (&(parent->units[n]) == this)
            return parent->next_theme(n+1);
            return nullptr;
        }

        unit* next ()
        {
            if (not kind == topic)
                return nullptr;

            if (not parent
            or  not parent->kind == theme)
                return nullptr;

            auto Theme = parent->next_theme(0);
            return Theme? Theme->first_topic():
                nullptr;
        }

        unit* last_theme ()
        {
            for (unit& u: reverse(units))
            if (u.kind == theme)
            return u.last_theme(); else
            return this;
            return this;
        }

        unit* prev_theme (int current)
        {
            for (int i=current; i >= 0 and i<units.size(); i--)
            if (units[i].kind == theme)
            return units[i].last_theme(); else
            return this;
            return prev_theme();
        }

        unit* prev_theme ()
        {
            if (not kind == theme)
                return nullptr;

            if (not parent
            or  not parent->kind == theme)
                return nullptr;

            for (int n=0; n < parent->units.size(); n++)
            if (&(parent->units[n]) == this)
            return parent->prev_theme(n-1);
            return nullptr;
        }

        unit* prev ()
        {
            if (not kind == topic)
                return nullptr;

            if (not parent
            or  not parent->kind == theme)
                return nullptr;

            auto Theme = parent->prev_theme();
            return Theme? Theme->first_topic():
                nullptr;
        }
    };
}