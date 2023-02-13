#pragma once
#include "studio.h"
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
        out << dark(bold("ONE: SCAN COURSE..."));

        content::logs::out = out;
        content::logs::err = err;
        content::out::course course("content");

        using ::studio::one::report::errors; errors.clear();
        using ::studio::one::report::anomal; anomal.clear();
        using ::studio::one::report::audiom; audiom.clear();
        using ::studio::one::report::videom; videom.clear();
        using ::studio::one::report::audiop; audiop.clear();
        using ::studio::one::report::videop; videop.clear();
        using ::studio::one::report::audioq; audioq.clear();
        using ::studio::one::report::videoq; videoq.clear();

        errors = course.errors;
        anomal = course.anomal;
        
        if (not errors.empty()) {
        err << red(bold("ONE ERRORS:"));
        err << errors; }

        using res = media::resource*;

        out << dark(bold("ONE: SCAN ENTRIES..."));

        std::unordered_map
        <str, array<res>> vocab;
        for (auto& entry: course.entries)
        for (str s: entry.vocabulary)
        vocab.emplace(s,array<res>{});

        out << dark(bold("ONE: SCAN RESOURCES..."));

        array<res> unused_resources;
        for (auto& r: data.resources)
        {
            array<str> sss;
            sss += r.title; if (r.kind == "video")
            sss += r.entries;
            for (str ss: sss)
            for (str s: ss.split_by("/"))
            {
                auto it = vocab.find(s);
                if (it != vocab.end())
                    it->second += &r;
                else
                if (r.usage > 5
                or  r.kind == "video")
                unused_resources += &r;
            }
        }

        out << dark(bold("ONE: CHECK FULFILMENT..."));

        for (auto [i, entry]: enumerate(course.entries))
        {
            bool en = entry.en.empty();
            bool uk = entry.uk.empty();
            bool us = entry.us.empty();
            bool vi = entry.opt.internal.contains("pix-");
            for (str& s: entry.vocabulary)
            for (res& r: vocab[s])
            {
                data.one_add(i, r);
                if (r->kind == "audio") {
                if (r->options.contains("uk")) uk = true;
                if (r->options.contains("us")) us = true; en = true; }
                if (r->kind == "video") vi = true;
            }
            if (not en) audiom += linked(entry.eng, entry.link);
            if (not uk) audiom += linked(entry.eng + red(bold(" uk")), entry.link);
            if (not us) audiom += linked(entry.eng + red(bold(" us")), entry.link);
            if (not vi) videom += linked(entry.eng, entry.link);
        }

        out << dark(bold("ONE: MAKE SUGGESTIONS..."));

        for (res r: unused_resources)
        {
            // r.keywords;

            if (r->kind == "audio") audioq += r->title;
            if (r->kind == "video") videoq += r->title;
        }

        /*
        std::unordered_map<res, array<str>> resources2entries;

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

        ::studio::one::report::save();
    }
}
