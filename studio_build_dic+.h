﻿#pragma once
#include "eng_parser.h"
#include "eng_unittest.h"
#include "media_data.h"
namespace studio::build::dic
{
    void compile
    (
        eng::vocabulary& vocabulary,
        array<int>& redirects,
        media::out::data& data
    )
    {
        auto& out = app::logs::report;
        auto& err = app::logs::errors;

        using res = media::resource*;
        std::unordered_map<int, array<res>> entries2resources;
        std::unordered_map<res, array<str>> resources2entries;

        out << dark(bold("DIC: SCAN RESOURCES..."));

        for (auto& r: data.resources)
        {
            if (r.title == "speaker.128x096"
            or  r.title == "player.black.play.64x64"
            or  r.title == "player.black.pause.64x64"
            or  r.title == "player.black.next.64x64"
            or  r.title == "player.black.stop.64x64"
            or  r.title == "icon.chevron.up.black.128x128"
            or  r.title == "icon.chevron.down.black.128x128"
            or  r.title == "icon.chevron.right.black.128x128"
            or  r.title == "icon.chevron.left.black.128x128"
            or  r.title == "icon.chevron.up.double.black.128x128"
            or  r.title == "icon.chevron.down.double.black.128x128"
            or  r.title == "icon.chevron.right.double.black.128x128"
            or  r.title == "icon.chevron.left.double.black.128x128"
            or  r.title == "icon.settings.black.192x192")
            {
                data.assets.insert(&r);
                continue;
            }

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

            entries += r.title;
            entries.try_erase("+");

            str s  = r.title; s.trimr("!?");
            if (s != r.title) entries += s;
            if (s.starts_with("a "    )) entries += s.from (2); else
            if (s.starts_with("an "   )) entries += s.from (3); else
            if (s.starts_with("the "  )) entries += s.from (4); else
            if (s.starts_with("to be ")) entries += s.from (7); else
            if (s.starts_with("to "   )) entries += s.from (3); else
            {}

            if (r.sense != ""
            and eng::list::sensitive.contains(r.sense))
            entries += r.sense;

            array<str> apostros; auto a = (char*)(u8"’");
            for(auto& e: entries) if (e.contains(a)) apostros += e;
            for(auto& e: apostros) e.replace_all(a, "'");
            entries += apostros;
            entries.deduplicate();

            for(auto& entry: entries)
            {
                entry.strip();
                auto index = vocabulary.index(entry);
                if (!index) continue;
                int n = *index;

                if (redirects[n] >= 0)
                n = redirects[n];

                entries2resources[n] += &r;
            }
        }

        out << dark(bold("DIC: CALCULATE FREQUENCIES..."));

        std::map<int, array<int>> frequency;
        std::map<int, array<int>> frequency_a;
        std::map<int, array<int>> frequency_v;
        for (auto& [entry, rr]: entries2resources)
        {
            int a = 0, v = 0;
            for (auto& r: rr)
            if (r->kind == "audio") a++; else
            if (r->kind == "video") v++;

            if (a > 0) frequency_a[a] += entry;
            if (v > 0) frequency_v[v] += entry;
            if (a+v>0) frequency[a+v] += entry;
        }
        out << purple(bold("AUDIO STATISTICS"));
        for (auto [n, entries]: std::ranges::reverse_view(frequency_a)) {
            std::ranges::sort(entries);
            str list; int nn = entries.size();
            for (int e: entries) { nn--; list +=
                "[" + html(vocabulary[e].title) + "] ";
                if (list.size() > 2*1024 and nn > 10) {
                    list += "<br>+" +
                    std::to_string(nn) +
                    " more";
                    break;
                }
            }
            out <<
            purple(bold(std::to_string(n))) + ": " +
            blue(list);
        }
        out << purple(bold("VIDEO STATISTICS"));
        for (auto [n, entries]: std::ranges::reverse_view(frequency_v)) {
            std::ranges::sort(entries);
            str list; int nn = entries.size();
            for (int e: entries) { nn--; list +=
                "[" + html(vocabulary[e].title) + "] ";
                if (list.size() > 2*1024 and nn > 10) {
                    list += "<br>+" +
                    std::to_string(nn) +
                    " more";
                    break;
                }
            }
            out <<
            purple(bold(std::to_string(n))) + ": " +
            blue(list);
        }

        out << dark(bold("DIC: LINK RESOURCES..."));

        int total_media = 0;

        for (auto [_, entries]: std::ranges::reverse_view(frequency))
        {
            for (int entry: entries)
            {
                std::map<int, array<res>> weighted_resources;

                for (auto r: entries2resources[entry])
                {
                    int weight = r->title.size();
                    int penalty = 10;

                    for (str e: r->entries)
                    {
                        e.strip();
                        auto index = vocabulary.index(entry);
                        if (!index) continue;
                        int n = *index;

                        if (redirects[n] >= 0) n =
                            redirects[n];

                        if (n == entry) { weight -= penalty; continue; }

                        auto it = entries2resources.find(n);
                        if (it == entries2resources.end())
                            weight += 2*penalty; else
                            weight += 1*penalty*
                            (100 - min(100, it->second.size()))/100;
                    }

                    weighted_resources[weight] += r;
                }

                int total_for_entry = 0;

                for (auto [weight, rr]: weighted_resources)
                {
                    if (total_for_entry > 99) break;
        
                    for (auto r: rr)
                    {
                        if (++total_for_entry > 99) break;
        
                        resources2entries[r] +=
                        vocabulary[entry].title;

                        data.dic_add(entry, r);
                    }
                }
            }
        }

        out << dark(bold("DIC: CHECK USAGE..."));

        for (auto& r: data.resources)
        {
            auto it = resources2entries.find(&r);
            if (it != resources2entries.end())
                r.usage = it->second.size();

            if (it == resources2entries.end()
            and not r.options.contains("sic!")
            and not r.options.contains("==")
            and not r.options.contains("=")
            and not data.assets.contains(&r)) {
                str ee = str(r.entries, "] [");
                str fn = r.path.string();
                err << yellow(html(fn)) +
                red(" [" + ee + "]");
                continue; }

            if (not data.new_ones.contains(&r))
                continue;
        
            array<str> accepted = it->second;
            array<str> rejected;
        
            for (auto& entry : r.entries)
            if (not accepted.contains(entry))
                    rejected += entry;
        
            out <<
            purple(html(r.title)) +
            blue  (html(" [" + str(accepted,  "] [") + "]")) +
            red   (html(" [" + str(rejected,  "] [") + "]")) +
            green (html(" {" + str(r.options, "} {") + "}"));
        }
    }
}
