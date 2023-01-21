#pragma once
#include "eng_parser.h"
#include "eng_unittest.h"
#include "media_data.h"
namespace studio::build::dic
{
    void compile (array<media::resource> const& resources, gui::console& report)
    {
        media::
        data::out::storage storage("../data/media");
        dat::out::file entry_index("../data/media/entry_index.dat");
        dat::out::file media_index("../data/media/media_index.dat");
        dat::out::file locationary("../data/media/locationary.dat");
        dat::out::file assets_data("../data/media/assets.dat");
        eng::vocabulary vocabulary("../data/vocabulary.dat");

        array<int> redirects;
        redirects.resize(vocabulary.size());
        dat::in::pool pool("../data/dictionary_indices.dat");
        for (int i=0; i<vocabulary.size(); i++) {
            eng::dictionary::index index; index << pool;
            redirects[i] = index.redirect; }

        using res = media::resource const*;
        std::unordered_map<int, array<res>> entries2resources;
        std::unordered_map<res, array<str>> resources2entries;
        std::map<int32_t, std::map<int32_t, str>> locations;
        std::set<res> new_ones;
        std::set<res> assets;

        constexpr auto html = doc::html::encoded;

        for (auto& r: resources)
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
                assets.insert(&r);
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

            entries.try_erase("+");
            entries += r.title;

            if (r.sense != ""
            and eng::list::sensitive.contains(r.sense))
            entries += r.sense;

            str s  = r.title; s.trimr("!?");
            if (s != r.title) entries += s;
            if (s.starts_with("a "    )) entries += s.from (2); else
            if (s.starts_with("an "   )) entries += s.from (3); else
            if (s.starts_with("the "  )) entries += s.from (4); else
            if (s.starts_with("to be ")) entries += s.from (7); else
            if (s.starts_with("to "   )) entries += s.from (3); else
            {}

            array<str> apostros; auto a = (char*)(u8"’");
            for(auto& entry: entries) if (entry.contains(a)) apostros += entry;
            for(auto& entry: apostros) entry.replace_all(a, "'");
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
        report << purple(bold("AUDIO STATISTICS"));
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
            report <<
            purple(bold(std::to_string(n))) + ": " +
            blue(list);
        }
        report << purple(bold("VIDEO STATISTICS"));
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
            report <<
            purple(bold(std::to_string(n))) + ": " +
            blue(list);
        }

        std::multimap<int, int> entry_media; int total_media = 0;

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
                        if (e.ends_with("}")) {
                        str sense; e.split_by("{",
                            e, sense); e.strip(); }

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
        
                        auto[location, new_one] = storage.add(*r);

                        if (new_one) {
                            new_ones.insert(r);
                            report << html(r->path.string()); }

                        locations
                        [location.source]
                        [location.offset] =
                            r->path.string();

                        resources2entries[r] +=
                        vocabulary[entry].title;
        
                        media_index << r->kind;
                        media_index << r->title;
                        media_index << r->sense;
                        media_index << r->comment;
                        media_index << r->credit;
                        media_index << r->options;
                        media_index << location;
                        total_media++;

                        entry_media.emplace(entry, total_media-1);
                    }
                }
            }
        }
        
        entry_index << total_media;
        entry_index << (int)(entry_media.size());
        for (auto [entry, media] : entry_media) {
            entry_index << entry;
            entry_index << media;
        }
        
        assets_data << (int)(assets.size());
        for (auto & r : assets) {
            assets_data << r->title;
            assets_data << dat::in::bytes(r->path).value();
        }

        for (auto& r: resources)
        {
            auto it = resources2entries.find(&r);
            if (it == resources2entries.end()
            and not r.options.contains("sic!")
            and not r.options.contains("==")
            and not r.options.contains("=")
            and not assets.contains(&r)) {
                report << yellow(html(r.path.string())) +
                red(" [" + str(r.entries, "] [") + "]");
                continue; }

            if (not new_ones.contains(&r))
                continue;
        
            array<str> accepted = it->second;
            array<str> rejected;
        
            for (auto& entry : r.entries)
            if (not accepted.contains(entry))
                    rejected += entry;
        
            report <<
            purple(html(r.title)) +
            blue  (html(" [" + str(accepted,  "] [") + "]")) +
            red   (html(" [" + str(rejected,  "] [") + "]")) +
            green (html(" {" + str(r.options, "} {") + "}"));
        }

        for (auto& [source, map] : locations)
        for (auto& [offset, path] : map) {
            locationary << source;
            locationary << offset;
            locationary << path;
        }
    }
}
