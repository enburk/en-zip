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

    const std::set<str> search_symbols = {"/","@","#","&","(",")","=",ellipsis};

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

        void apply (auto f)
        {
            f(*this); for (unit& u: units) u.apply(f);
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

                aux::shuffle(i, j);
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

        unit* forward (auto function)
        {
            unit* current = this;
            while (current->parent)
            {
                unit* u = current->parent;
                auto i = u->units.find_if([current](unit& x){ return &x == current; });
                if (i < u->units.end())
                    for (i++; i < u->units.end(); i++)
                        if (unit* x = function(*i))
                            return x;

                current = u;
            }
            return nullptr;
        }

        unit* backward (auto function)
        {
            unit* current = this;
            while (current->parent)
            {
                unit* u = current->parent;
                auto i = u->units.find_if([current](unit& x){ return &x == current; });
                if (i > u->units.begin())
                    for (i--; i >= u->units.begin(); i--)
                        if (unit* x = function(*i))
                            return x;

                current = u;
            }
            return nullptr;
        }

        unit* first_subtopic ()
        {
            if (kind == topic) return this;
            if (kind != theme) return nullptr;
            for (unit& u: units)
                if (unit* x = u.first_subtopic())
                    return x;
            return nullptr;
        }

        unit* last_subtopic ()
        {
            if (kind == topic) return this;
            if (kind != theme) return nullptr;
            for (unit& u: reverse(units))
                if (unit* x = u.last_subtopic())
                    return x;
            return nullptr;
        }

        unit* prev_topic ()
        {
            if (not kind == topic) return nullptr;
            return backward([](unit& u){ return
                u.last_subtopic(); });
        }

        unit* next_topic ()
        {
            if (not kind == topic) return nullptr;
            return forward([](unit& u){ return
                u.first_subtopic(); });
        }

        unit* prev_theme_last_topic ()
        {
            if (kind == topic and parent) return parent->prev_theme_last_topic();
            if (kind != theme) return nullptr;
            return backward([](unit& u){ return
                u.last_subtopic(); });
        }

        unit* prev_theme_first_topic ()
        {
            unit* x = prev_theme_last_topic();
            if (x and x->parent)
                for (unit& u: x->parent->units)
                    if (u.kind == topic)
                        return &u;
            return nullptr;
        }

        unit* first_subtheme_with_topic ()
        {
            for (unit& u: units)
            {
                if (u.kind == topic)
                    return this;
                if (u.kind == theme)
                    if (unit* x = u.first_subtheme_with_topic())
                        return x;
            }
            return nullptr;
        }

        unit* next_theme_first_topic ()
        {
            if (kind == topic and parent) return parent->next_theme_first_topic();
            if (kind != theme) return nullptr;
            for (unit& u: units)
                if (u.kind == theme)
                    if (unit* x = u.first_subtheme_with_topic())
                        return x->first_subtopic();
            return forward([](unit& u){ return
                u.first_subtopic(); });
        }
    };
}