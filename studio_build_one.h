#pragma once
#include "studio_build_one+.h"
namespace studio::one
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
        if (true) sys::out::file("../data/course.dat") << course.root;
        if (true) sys::out::file("../data/course_entries.dat") << course.entries;
        if (true) sys::out::file("../data/course_searchmap.dat") << course.searchmap;

        report::clear();
        report::errors = course.errors;
        report::anomal = course.anomal;
        
        if (not report::errors.empty()) {
        err << red(bold("ONE ERRORS:"));
        err << report::errors; }

        out << dark(bold("ONE: SCAN ENTRIES..."));

        hashset<str> form_course_vocabulary;
        hashmap<str, voc> course_vocabulary;
        for (auto& entry: course.entries)
        for (str s: entry.vocabulary)
        {
            course_vocabulary[s].
            entries += &entry;

            for (str f: forms(s))
            form_course_vocabulary.
                emplace(f);
        }

        std::unordered_set<str> sensitive;
        for (auto [s,voc]: course_vocabulary)
        {
            str entry = s;
            str sense = entry.extract_from("@");
            if (sense != "") sensitive.
            emplace(entry);

            if (voc.entries.size() < 2) continue;
            for (ent entry: voc.entries)
            report::duples += link(entry);
            report::duples += "";
        }

        out << dark(bold("ONE: SCAN RESOURCES..."));

        std::unordered_set<res> unused_resources;
        for (auto& r: data.resources)
        {
            str w = simple(r.abstract);

            array<str>
            resource_vocabulary;
            resource_vocabulary += w;
            {
                auto& v = resource_vocabulary;
                if (w.starts_with("a "    )) v += str(w.from(2)) + "@noun"; else
                if (w.starts_with("an "   )) v += str(w.from(3)) + "@noun"; else
                if (w.starts_with("the "  )) v += str(w.from(4)) + "@noun"; else
                if (w.starts_with("to "   )) v += str(w.from(3)) + "@verb"; else
                {}
            }

            if (r.kind == "video")
            {
                if (r.options.contains("asset")
                or  r.options.contains("noqrop"))
                    continue;

                if (r.entries.contains("+"))
                resource_vocabulary +=
                    eng::parser::entries(
                    vocabulary, r.title,
                    r.options.contains
                    ("Case"));

                resource_vocabulary +=
                    r.entries;
            }

            bool used = false;
            for (str ss: resource_vocabulary)
            for (str s: ss.split_by("/"))
            {
                auto it = course_vocabulary.find(s);
                if (it != course_vocabulary.end())
                    it->second.resources += &r,
                    used = true;

                str sense = s.extract_from("@");
                if (sense != "") sensitive.
                    emplace(s);
            }

            if (used or
                form_course_vocabulary.
                contains(w))
                continue;

            int spaces = 0;
            for (char c: w)
            if  (c == ' ')
                spaces++;

            int k =
            spaces >= 2 ? 10:
            spaces == 1 ? 15: 19;

            if (r.weight < k
            or  r.sense != ""
            or  r.kind == "video")
            unused_resources.
                emplace(&r);
        }

        std::unordered_set<res> single_videos;

        for (auto& [s,voc]: course_vocabulary)
        {
            int n = 0;
            res v = 0;
            for (res r: voc.resources)
            if (r->kind == "video") n++, v = r;
            if (n == 1) single_videos.emplace(v);
        }

        for (auto& [s,voc]: course_vocabulary)
        {
            int n = 0;
            for (res r: voc.resources)
            if (r->kind == "video") n++;
            if (n <= 1) continue;

            array<res> aa;
            array<res> vv;
            for (res r: voc.resources)
            if (r->kind == "video")
            vv += r; else
            aa += r;

            vv.erase_if([&single_videos](res r){
            return single_videos.contains(r); });

            if (not vv.empty())
            voc.resources = aa * vv;
        }

        out << dark(bold("ONE: CHECK FULFILMENT..."));

        auto fullfill = [](array<str>& units, str word)
        {
            units.erase_if([word](str& unit)
            {
                for (str s: unit.split_by("|"))
                if (s == word) return true;
                return false;
            });
        };

        for (auto [i, entry]: enumerate(course.entries))
        {
            bool nopixed = entry.opt.internal.contains("pix-");
            bool noaudio = entry.opt.internal.contains("audio-");

            auto en = noaudio ? array<str>{} : entry.en;
            auto uk = noaudio ? array<str>{} : entry.uk;
            auto us = noaudio ? array<str>{} : entry.us;
            bool vi = nopixed;

            for (str& s: entry.vocabulary)
            for (res& r: course_vocabulary[s].resources)
            {
                if (r->kind == "video"
                and nopixed) continue;

                if (r->kind == "audio"
                and noaudio) continue;

                data.one_add(i, r);

                str w = simple(r->abstract);

                if (r->kind == "audio") {
                if (r->options.contains("uk")) fullfill(uk, w);
                if (r->options.contains("us")) fullfill(us, w);
                /*        in any case       */ fullfill(en, w); }
                if (r->kind == "video") vi = true;
            }
            str s = html(entry.eng);
            str ens = red(bold(" en: " + html(str(en, ", "))));
            str uks = red(bold(" uk: " + html(str(uk, ", "))));
            str uss = red(bold(" us: " + html(str(us, ", "))));
            if (not en.empty()) report::audiom += linked(s + ens, entry.link);
            if (not uk.empty()) report::audiom += linked(s + uks, entry.link);
            if (not us.empty()) report::audiom += linked(s + uss, entry.link);
            if (not vi        ) report::videom += linked(s + " ", entry.link);
        }

        out << dark(bold("ONE: MAKE SUGGESTIONS..."));

        hashmap<str,
        array<res>> unused_resources_vocabulary;
        for (res r: unused_resources)
        {
            auto entries = r->entries;

            if ((r->kind == "audio"
            and entries.size() == 0
            and not r->options.contains("=")
            and not r->options.contains("=="))
            or  entries.contains("+"))
                entries += eng::parser::entries(
                    vocabulary, r->title,
                    r->options.contains
                    ("Case"));
            else
            if (r->title.contains("/"))
            for (str s: r->title.split_by("/"))
                entries += s;

            entries.try_erase("+");

            for (str s: entries)
            unused_resources_vocabulary[s] += r;
        }

        hashset<str> current_vocabulary;

        for (auto& entry: course.entries)
        {
            array<res> audios;
            array<res> videos;

            for (str& s: entry.vocabulary)
            {
                for (str word: eng::parser::entries(
                    vocabulary, s.upto_first("@"), true))
                    current_vocabulary.emplace(word);

                auto it = unused_resources_vocabulary.find(s);
                if (it == unused_resources_vocabulary.end())
                    continue;

                for (res r: it->second)
                {
                    // 's 't 're 'll
                    if (eng::list::contractionparts.
                        contains(r->title))
                        continue;

                    // if hasn't been deleted
                    if (not unused_resources.
                        contains(r))
                        continue;

                    // if all words are known
                    bool well_known = true;
                    for (str x: r->entries)
                    if  (not current_vocabulary.contains(x))
                        well_known = false;

                    if (not well_known)
                        continue;

                    if (r->kind == "audio") audios += r;
                    if (r->kind == "video") videos += r;
                    unused_resources.erase(r); // delete it
                }
            }

            if (not audios.empty())
            {
                report::audiop += link(entry);
                for (res r: audios)
                report::audiop += 
                dark(html(r->
                     full()));
            }
            if (not videos.empty())
            {
                report::videop += link(entry);
                for (res r: videos)
                report::videop +=
                dark(html(r->
                     full()));
            }
        }

        std::multimap<int, res>
            weighted_unused_resources;
        for (res r:  unused_resources)
            weighted_unused_resources.
            emplace(r->weight, r);

        for (auto[weight, r]: weighted_unused_resources)
        {
            str s = r->abstract;

            // 's 't 're 'll
            if (eng::list::contractionparts.
                contains(s))
                continue;

            if (r->kind == "audio")
                report::audioq +=
                r->full();

            if (r->kind == "video")
                report::videoq +=
                r->full();
        }

        for (auto& entry: course.entries)
        for (str s: entry.vocabulary)
        if  (sensitive.contains(s))
            report::errors += bold(
            red("sensless: ")) +
            link(entry);

        report::save();
    }
}
