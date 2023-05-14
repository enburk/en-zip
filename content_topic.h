#pragma once
#include <filesystem>
#include "content_entry.h"
namespace content::out
{
    struct topic
    {
        std::filesystem::path path;
        array<entry> entries;
        array<str> errors;
        array<str> anomal;

        topic (std::filesystem::path path, array<str> const& lines) : path(path)
        {
            int n = 0;
            for (auto& line: lines)
            {
                entry entry{line, path, n++};
                
                if (entry.anomaly != "") anomal += linked(
                    blue(monospace(std::format("{:2}: ",
                    entry.line+1))) + entry.eng,
                    entry.link);

                for (str error: entry.errors) errors += linked(
                    blue(monospace(std::format("{:2}: ",
                    entry.line+1))) + entry.eng + " " +
                    red(error),
                    entry.link);

                entries += std::move(entry);
            }
        }

        generator<unit> chains (array<entry>& accumulator)
        {
            unit chain;
            options opt;
            bool leading = true;
            bool ordered = true;
            int  order = 0;

            for (auto entry: entries)
            {
                if (entry.eng == ""
                and entry.rus == ""
                or  entry.eng.starts_with("~~~")
                or  entry.eng.starts_with("===")
                or  entry.eng.starts_with("---"))
                {
                    if (not
                    chain.units.empty()) co_yield chain;
                    chain.units.clear();
                    opt |= entry.opt;
                    if (leading)
                    chain.order++;
                    leading = false;
                    order = 0;
                }
                if (entry.eng == ""
                and entry.rus == "")
                {
                    ordered = false;
                    continue;
                }
                if (entry.eng.starts_with("~~~"))
                {
                    chain.units += unit{};
                    ordered = true;
                    continue;
                }
                if (entry.eng.starts_with("==="))
                {
                    chain.order++;
                    ordered = true;
                    continue;
                }
                if (entry.eng.starts_with("---"))
                {
                    ordered = true;
                    continue;
                }

                entry.opt |= opt;

                if (leading)
                    entry.opt.external
                    += "HEAD";

                unit unit;
                unit.order = order;
                if (ordered)
                    order++;

                unit.entry =
                accumulator.size();
                accumulator += entry;
                chain.units += unit;
            }
            if (not
            chain.units.empty())
            co_yield chain;
        }

        array<str> formatted () const
        {
            array<int> tabs1, tabs2;
            for (auto& entry: entries)
            {
                // eng... = rus... # opt
                // eng... = rus
                // eng... # opt
                // eng
                // = rus # opt
                // = rus
                // # opt
                // 
                int o =
                entry.comment == "" and
                entry.opt == options{} ? 0 : 1;
                int e = aux::unicode::length(entry.eng);
                int r = aux::unicode::length(entry.rus);
                if (e and (r and o)) tabs2 += e + r + 3;
                if (e and (r or  o)) tabs1 += e;
            }
            std::ranges::sort(tabs1);
            std::ranges::sort(tabs2);
            int a = tabs1.size();
            int b = tabs2.size();
            int tab1 = a ? tabs1[a*2/3] : 0;
            int tab2 = b ? tabs2[b*2/3] : 0;
            for (int i=a*2/3+1; i<a; i++) if (tabs1[i] < 30) tab1 = tabs1[i];
            for (int i=b*2/3+1; i<b; i++) if (tabs2[i] < 80) tab2 = tabs2[i];
            array<str> ss;
            for (auto& entry: entries)
            ss += entry.formatted(tab1, tab2);
            return ss;
        }
    };
}
