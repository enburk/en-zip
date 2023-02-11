#pragma once
#include <filesystem>
#include "content_entry.h"
namespace content
{
    struct topic
    {
        array<entry> entries;
        array<str> anomal1;
        array<str> anomal2;
        array<str> errors;

        topic (array<str> const& lines)
        {
            int n = 0;
            for (auto& line: lines)
            {
                entry entry{line, n++};
                
                if (entry.anomaly == "Br/Am") anomal1 +=
                    blue(monospace(std::format("{:2}: ",
                    entry.line))) + entry.eng;
                else
                if (entry.anomaly != "") anomal2 +=
                    blue(monospace(std::format("{:2}: ",
                    entry.line))) + entry.eng;

                for (str error: entry.errors) errors +=
                    blue(monospace(std::format("{:2}: ",
                    entry.line))) + entry.eng + " " +
                    red(error);

                entries += std::move(entry);
            }
        }

        generator<unit> chains (array<entry>& accumulator)
        {
            unit chain;
            options opt;
            bool chain_ordered = true;
            bool entry_ordered = true;
            int  entry_order = 0;

            for (auto entry: entries)
            {
                if (entry.eng == ""
                and entry.rus == ""
                or  entry.eng.starts_with("===")
                or  entry.eng.starts_with("---"))
                {
                    if (not
                    chain.units.empty()) co_yield chain;
                    chain.units.clear();
                    if (chain_ordered)
                        chain.order++;
                    opt |= entry.opt;
                    entry_order = 0;
                }
                if (entry.eng == ""
                and entry.rus == "")
                {
                    chain_ordered = false;
                    entry_ordered = false;
                    continue;
                }
                if (entry.eng.starts_with("==="))
                {
                    chain_ordered = true;
                    entry_ordered = true;
                    continue;
                }
                if (entry.eng.starts_with("---"))
                {
                    chain_ordered = false;
                    entry_ordered = true;
                    continue;
                }

                entry.opt |= opt;

                unit unit;
                unit.order = entry_order;
                if (entry_ordered)
                    entry_order++;

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
