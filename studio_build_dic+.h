﻿#pragma once
#include "eng_parser.h"
#include "eng_unittest.h"
#include "media_data.h"
namespace studio::dic
{
    namespace report
    {
        array<str> errors;
        array<str> usages;
        array<str> statts;
        void load ()
        {
            std::filesystem::path dir = "../data/report";
            errors = sys::optional_text_lines(dir/"dic_errors.txt");
            usages = sys::optional_text_lines(dir/"dic_usages.txt");
            statts = sys::optional_text_lines(dir/"dic_statts.txt");
            usages.resize(1000);
        }
        void save ()
        {
            std::filesystem::path dir = "../data/report";
            sys::write(dir/"dic_errors.txt", errors);
            sys::write(dir/"dic_usages.txt", usages);
            sys::write(dir/"dic_statts.txt", statts);
        }
        void clear()
        {
            errors.clear();
            usages.clear();
            statts.clear();
        }
    }

    void compile
    (
        eng::vocabulary& vocabulary,
        array<int>& redirects,
        media::out::data& data
    )
    {
        report::clear();
        auto& out = app::logs::report;
        auto& err = app::logs::errors;

        using res = media::resource*;
        std::unordered_map<int, array<res>> entries2resources;
        std::unordered_map<res, array<int>> resources2entries;
        std::unordered_map<res, array<str>> resources2titles;

        std::unordered_map<str,
        std::unordered_map<str,
            res>> abstractmap;

        out << dark(bold("DIC: SCAN RESOURCES..."));

        for (auto& r: data.resources)
        {
            if (r.options.contains("asset")) {
                data.assets.insert(&r);
                continue; }

            for (str e: r.Entries())
            if (e.contains(one_of ("{}[]")))
                report::errors += yellow("{}[]: ") +
                linked(html(r.path.stem().string()),
                "file://" + r.path.string());

            abstractmap[r.abstract][simple(r.title)] = &r;

            array<str> entries = r.entries;

            if ((r.kind == "audio"
            and entries.size() == 0
            and not r.options.contains("=")
            and not r.options.contains("=="))
            or  entries.contains("+"))
                entries += eng::parser::entries(
                    vocabulary, r.title,
                    r.options.contains
                    ("Case"));
            else
            if (r.title.contains("/"))
            for (str s: r.title.split_by("/"))
                entries += s;

            if (r.kind == "audio"
            and r.sense != ""
            and eng::list::sensitive.contains(
                r.sense)) entries +=
                r.sense;

            entries += r.title;
            entries.try_erase("+");

            str s  = r.title; s.trimr(".!?");
            if (s != r.title) entries += s;
            if (s.starts_with("a "    )) entries += s.from (2); else
            if (s.starts_with("an "   )) entries += s.from (3); else
            if (s.starts_with("the "  )) entries += s.from (4); else
            if (s.starts_with("to be ")) entries += s.from (7); else
            if (s.starts_with("to "   )) entries += s.from (3); else
            if (s.starts_with("His "  )) entries += "one's" + str(s.from(3)); else
            if (s.starts_with("Her "  )) entries += "one's" + str(s.from(3)); else
            {}

            array<str> apostros; str a = u8"’";
            for(auto& e: entries) if (e.contains(a)) apostros += e;
            for(auto& e: apostros) e.replace_all(a, "'");
            entries += apostros;
            entries.deduplicate();

            for(auto& entry: entries)
            {
                entry.strip();
                str sense = entry.extract_from("@");
                auto index = vocabulary.index(entry);
                if (!index) continue;
                int n = *index;

                if (redirects[n] >= 0)
                n = redirects[n];

                entries2resources[n] += &r;
            }
        }

        out << dark(bold("DIC: CALCULATE FREQUENCIES..."));

        std::map<int, int>  entry2frequency;
        std::map<int, array<int>> frequency;
        std::map<int, array<int>> frequency_a;
        std::map<int, array<int>> frequency_v;
        for (auto& [entry, rr]: entries2resources)
        {
            int a = 0, v = 0;
            for (auto& r: rr)
            if (r->kind == "audio") a++; else
            if (r->kind == "video") v++;

            if (a > 1000) a = a/100*100; else
            if (a > 100) a = a/10*10;

            if (a > 0) frequency_a[a] += entry;
            if (v > 0) frequency_v[v] += entry;
            if (a+v>0) frequency[a+v] += entry;

            entry2frequency[entry] = a + 2*v;
        }
        report::statts += purple(bold("AUDIO STATISTICS"));
        for (auto [n, entries]: std::ranges::reverse_view(frequency_a)) {
            std::ranges::sort(entries);
            str list; int nn = entries.size();
            for (int e: entries) { nn--;
                str s = vocabulary[e].title;
                list += "[" + linked(html(s), s) + "] ";
                if (list.size() > 2*1024 and nn > 10) {
                    list += "<br>+" +
                    std::to_string(nn) +
                    " more";
                    break;
                }
            }
            report::statts +=
            purple(bold(std::to_string(n))) + ": " +
            blue(list);
        }
        report::statts += purple(bold("VIDEO STATISTICS"));
        for (auto [n, entries]: std::ranges::reverse_view(frequency_v)) {
            std::ranges::sort(entries);
            str list; int nn = entries.size();
            for (int e: entries) { nn--;
                str s = vocabulary[e].title;
                list += "[" + linked(html(s), s) + "] ";
                if (list.size() > 2*1024 and nn > 10) {
                    list += "<br>+" +
                    std::to_string(nn) +
                    " more";
                    break;
                }
            }
            report::statts +=
            purple(bold(std::to_string(n))) + ": " +
            blue(list);
        }
        out << report::statts;

        out << dark(bold("DIC: LINK RESOURCES..."));

        int total_media = 0;

        for (auto [_, entries]: reverse(frequency))
        {
            for (int entry: entries) // more frequent first
            {
                std::map<int, array<res>> weighted_resources;

                for (auto r: entries2resources[entry])
                {
                    int weight = r->title.size();
                    int penalty = 10;

                    for (str e: r->entries)
                    {
                        e.strip();
                        auto index = vocabulary.index(e);
                        if (!index) continue;
                        int n = *index;

                        if (redirects[n] >= 0) n =
                            redirects[n];

                        if (n == entry) { weight -= penalty; continue; }

                        auto it = entries2resources.find(n);
                        if (it == entries2resources.end())
                            weight += 2*penalty; else
                            weight += 1*penalty*
                            (100 - min(100,
                            it->second.size()))
                            /100;
                    }

                    weighted_resources[weight] += r;
                }

                array<res> total_for_entry;

                for (auto [weight, rr]: weighted_resources)
                {
                    if (total_for_entry.size() > 99) break;
        
                    for (auto r: rr)
                    {
                        // doubles are possible because of redirects
                        if (total_for_entry.contains(r)) continue; total_for_entry += r;
                        if (total_for_entry.size() > 99) break;
        
                        resources2entries[r] += entry;
                        resources2titles [r] +=
                        vocabulary[entry].title;

                        data.dic_add(entry, r);
                    }
                }
            }
        }

        out << dark(bold("DIC: CALCULATE WEIGHTS..."));

        for (auto& [r, entries] : resources2entries)
        {
            int weight = r->title.size();
            int penalty = 100;

            for (int entry: entries)
            {
                int divider = 1;
                auto it = entry2frequency.find(entry);
                if (it != entry2frequency.end())
                    divider += it->second;

                weight += penalty/divider;
            }

            r->weight = weight;
        }

        out << dark(bold("DIC: CHECK USAGE..."));

        for (auto& r: data.resources)
        {
            auto it = resources2titles.find(&r);
            if (it != resources2titles.end())
                r.usage[0] = it->second.size();

            if (it == resources2titles.end()
            and not r.options.contains("asset")
            and not r.options.contains("sic!")
            and not r.options.contains("==")
            and not r.options.contains("=")
            and not data.assets.contains(&r)) {
                str ee = str(r.entries, "] [");
                str path = str(r.path);
                report::errors +=
                yellow("dic unused:") +
                linked(html(path),
                "file://" + path) +
                red(" [" + ee + "]");
                continue; }

            if (not data.new_ones.contains(&r))
                continue;
        
            array<str> options = r.options;
            array<str> accepted = it->second;
            array<str> rejected;
        
            for (auto& entry : r.entries)
            if (not accepted.contains(entry))
                    rejected += entry;

            for (str& s: accepted) s = linked(html(s), s);
            for (str& s: rejected) s = html(s);

            str title = linked(
                html(r.title),
                "file://" +
                str(r.path));
        
            report::usages += purple(title) +
            blue  (" [" + str(accepted,  "] [") + "]") +
            red   (" [" + str(rejected,  "] [") + "]") +
            green (" {" + str(r.options, "} {") + "}");
        }

        for (auto& [s, map]: abstractmap) if (map.size() > 1)
        {
            int audios = 0;
            for (auto& [t, r]: map)
            if (r->kind == "audio") audios++;
            if (audios < 2) continue;

            report::errors +=
            red(bold("conflicting abstracts:"));
            for (auto& [t, r]: map)
            if (r->kind == "audio")
            report::errors +=
            linked(html(r->abstract),
            "file://" + str(r->path));
        }
    }

    void reportage ()
    {
        auto& out = app::logs::report;
        auto& err = app::logs::errors;

        out << report::usages;
        err << report::errors;

        report::save();
    }
}
