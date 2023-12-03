#pragma once
#include "studio_build_one+.h"
namespace studio::one
{
    void generate_debug_theme
    (
        content::out::course& course,
        array<std::pair<ent, array<res>>>& multivideo,
        hashmap<res, array<ent>>& multientry,
        media::out::data& data
    )
    {
        auto& debug = course.root.add_theme(99, "Debug");
        {
            auto& anomal = debug.
            add_theme(10, "Anomal").
            add_topic(00, "anomal").
            add_chain(00);

            for (auto [i,entry]: enumerate(course.entries))
            if (entry.anomaly != "")
                anomal.add_leaf(i).
                entry = i;
        }
        {
            auto& multi = debug.add_theme(20, "Multivideo"); multi.units.reserve(2);
            auto& oldes = multi.add_theme(10, "Old").add_topic(00, "old").add_chain(00);
            auto& newes = multi.add_theme(20, "New").add_topic(00, "new").add_chain(00);

            hashmap<str, array<str>> old_ones;

            path dir = "../data/report";
            path dir_old = dir / "multivideo_old";
            path dir_new = dir / "multivideo_new";
            std::filesystem::remove_all(dir_new);
            for (path path: sys::files (dir_old))
            {
                array<str> lines = sys::optional_text_lines(path);
                while (not lines.empty())
                {
                    if (lines.front().empty()) break;
                    int n = std::stoi(lines.front());
                    if (n < 0 or n+2 >= lines.size())
                        throw std::runtime_error(
                        "multivideo_old: n = "+str(n)+
                        " size = "+str(lines.size()));

                    auto& ress =
                    old_ones[lines[1]];
                    ress.reserve(n);
                    ress.clear();

                    for (int i=0; i<n; i++)
                    ress += lines[i+2];

                    lines.upto(n+2).erase();
                }
            }

            array<str> list; int listn = 1;
            str date = aux::format("{%Y-%m-%d %H.%M.%S} ",
                std::chrono::system_clock::now());

            for (auto [entry, ress]: multivideo)
            {
                bool old = false;
                str s = entry->formatted(30,80);
                auto it = old_ones.find(s);
                if (it != old_ones.end()
                and it->second.size() == ress.size())
                {
                    old = true;
                    for (int i=0; i<ress.size(); i++)
                    if (it->second[i] != ress[i]->id)
                    old = false;
                }

                if (not old)
                list += std::to_string(ress.size()),
                list += s;

                for (res r: ress)
                {
                    int n = course.entries.size(); if (old)
                    oldes.add_leaf(oldes.units.size()).entry = n; else
                    newes.add_leaf(newes.units.size()).entry = n;
                    course.entries += *entry;
                    data.one_add(n, r);

                    if (not old)
                    list += r->id;
                }

                if (list.size() >= 100)
                sys::write(dir_new / (date +
                str(listn++).right_aligned(6, '0')), list),
                list.clear();
            }

            if (list.size() >= 0)
            sys::write(dir_new / (date +
            str(listn++).right_aligned(6, '0')), list),
            list.clear();
        }
        {
            auto& multi = debug.add_theme(20, "Multientry"); multi.units.reserve(2);
            auto& oldes = multi.add_theme(10, "Old").add_topic(00, "old").add_chain(00);
            auto& newes = multi.add_theme(20, "New").add_topic(00, "new").add_chain(00);

            hashmap<str, array<str>> old_ones;

            path dir = "../data/report";
            path dir_old = dir / "multientry_old";
            path dir_new = dir / "multientry_new";
            std::filesystem::remove_all(dir_new);
            for (path path: sys::files (dir_old))
            {
                array<str> lines = sys::optional_text_lines(path);
                while (not lines.empty())
                {
                    if (lines.front().empty()) break;
                    int n = std::stoi(lines.front());
                    if (n < 0 or n+2 >= lines.size())
                        throw std::runtime_error(
                        "multivideo_old: n = "+str(n)+
                        " size = "+str(lines.size()));

                    auto& ress =
                    old_ones[lines[1]];
                    ress.reserve(n);
                    ress.clear();

                    for (int i=0; i<n; i++)
                    ress += lines[i+2];

                    lines.upto(n+2).erase();
                }
            }

            array<str> list; int listn = 1;
            str date = aux::format("{%Y-%m-%d %H.%M.%S} ",
                std::chrono::system_clock::now());

            for (auto [r, ents]: multientry)
            {
                if (ents.size() < 2)
                    continue;

                if (ents.size() == 2
                and ents[0]->eng ==
                    ents[1]->eng)
                    continue;

                bool old = false;
                str s = r->id;
                auto it = old_ones.find(s);
                if (it != old_ones.end()
                and it->second.size() == ents.size())
                {
                    old = true;
                    for (int i=0; i<ents.size(); i++)
                    if (it->second[i] != ents[i]->formatted(30,80))
                    old = false;
                }

                if (not old)
                list += std::to_string(ents.size()),
                list += s;

                for (ent e: ents)
                {
                    int n = course.entries.size(); if (old)
                    oldes.add_leaf(oldes.units.size()).entry = n; else
                    newes.add_leaf(newes.units.size()).entry = n;
                    course.entries += *e;
                    data.one_add(n, r);

                    if (not old)
                    list += e->formatted(30,80);
                }

                if (list.size() >= 100)
                sys::write(dir_new / (date +
                str(listn++).right_aligned(6, '0')), list),
                list.clear();
            }

            if (list.size() >= 0)
            sys::write(dir_new / (date +
            str(listn++).right_aligned(6, '0')), list),
            list.clear();
        }
    }
}
