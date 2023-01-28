#pragma once
#include <filesystem>
#include "content_chain.h"
namespace content
{
    struct topic
    {
        str name;
        std::filesystem::path path;
        array<entry> entries;

        topic () = default;
        topic (array<str> const& lines)
        {
            for (str line: lines)
            entries += entry{line};
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
