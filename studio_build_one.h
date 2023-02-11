#pragma once
#include "eng_parser.h"
#include "media_data.h"
#include "content_course.h"
#include "studio_app_one_reports.h"
namespace studio::build::one
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

        content::logs::out = out;
        content::logs::err = err;
        content::out::course course("content");

        using ::studio::one::report::anomal1; anomal1.clear();
        using ::studio::one::report::anomal2; anomal2.clear();
        using ::studio::one::report::audiom ; audiom .clear();
        using ::studio::one::report::videom ; videom .clear();
        using ::studio::one::report::audiop ; audiop .clear();
        using ::studio::one::report::videop ; videop .clear();
        using ::studio::one::report::audioq ; audioq .clear();
        using ::studio::one::report::videoq ; videoq .clear();

        anomal1 << course.anomal1;
        anomal2 << course.anomal2;


        using res = media::resource*;
        std::unordered_map<int, array<res>> en;
        std::unordered_map<int, array<res>> uk;
        std::unordered_map<int, array<res>> us;
        std::unordered_map<int, array<res>> videos;

        for (auto& entry: course.entries)
        {
        }

        /*
        std::unordered_map<res, array<str>> resources2entries;

        constexpr auto html = doc::html::encoded;

        out << dark(bold("DIC: SCAN RESOURCES..."));

        for (auto& r: data.resources)
        {
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

            entries.try_erase("+");

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

                        data.add(r);
                        data.entrymap_dic.emplace
                        (entry, total_media++);
                    }
                }
            }
        }

        out << dark(bold("DIC: CHECK USAGE..."));

        for (auto& r: data.resources)
        {
            auto it = resources2entries.find(&r);
            if (it == resources2entries.end()
            and not r.options.contains("sic!")
            and not r.options.contains("==")
            and not r.options.contains("=")
            and not data.assets.contains(&r)) {
                err << yellow(html(r.path.string())) +
                red(" [" + str(r.entries, "] [") + "]");
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
        */
    }
}
