#pragma once
#include "app.h"
#include <unordered_set>
namespace studio::one
{
    namespace report
    {
        array<str> errors;
        array<str> anomal;
        array<str> audiom, audiop, audioq;
        array<str> videom, videop, videoq;
        void load ()
        {
            std::filesystem::path dir = "../data/report";
            errors = sys::in::optional_text_lines(dir/"one_errors.txt");
            anomal = sys::in::optional_text_lines(dir/"one_anomal.txt");
            audiom = sys::in::optional_text_lines(dir/"one_audiom.txt");
            videom = sys::in::optional_text_lines(dir/"one_videom.txt");
            audiop = sys::in::optional_text_lines(dir/"one_audiop.txt");
            videop = sys::in::optional_text_lines(dir/"one_videop.txt");
            audioq = sys::in::optional_text_lines(dir/"one_audioq.txt");
            videoq = sys::in::optional_text_lines(dir/"one_videoq.txt");
        }
        void save ()
        {
            std::filesystem::path dir = "../data/report";
            sys::out::write(dir/"one_errors.txt", errors);
            sys::out::write(dir/"one_anomal.txt", anomal);
            sys::out::write(dir/"one_audiom.txt", audiom);
            sys::out::write(dir/"one_videom.txt", videom);
            sys::out::write(dir/"one_audiop.txt", audiop);
            sys::out::write(dir/"one_videop.txt", videop);
            sys::out::write(dir/"one_audioq.txt", audioq);
            sys::out::write(dir/"one_videoq.txt", videoq);
        }
        void clear()
        {
            errors.clear();
            anomal.clear();
            audiom.clear();
            videom.clear();
            audiop.clear();
            videop.clear();
            audioq.clear();
            videoq.clear();
        }
    }

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

        using res = media::resource*;

        auto simple = [](str s)
        {
            s.replace_all(mdash, "---");
            s.replace_all(ndash, "--");
            s.replace_all(u8"’", "'");
            return s;
        };

        out << dark(bold("ONE: SCAN ENTRIES..."));

        std::unordered_map
        <str, array<res>> course_vocabulary;
        for (auto& entry: course.entries)
        for (str s: entry.vocabulary)
            course_vocabulary.emplace(
            s, array<res>{});

        std::unordered_set<str> sensitive;
        for (auto [s,rr]: course_vocabulary)
        {
            str entry = s;
            str sense = entry.extract_from("@");
            if (sense != "") sensitive.
            emplace(entry);
        }

        for (auto& entry: course.entries)
        for (str s: entry.vocabulary)
        if (sensitive.contains(entry.eng))
            report::errors += linked(
                red(bold("sensless: ")) +
                html(entry.eng),
                    entry.link);

        out << dark(bold("ONE: SCAN RESOURCES..."));

        std::unordered_set<res> unused_resources;
        for (auto& r: data.resources)
        {
            array<str>
            resource_vocabulary;
            resource_vocabulary +=
            simple(r.abstract);

            if (r.kind == "video")
            {
                if (r.options.
                    contains("noqrop"))
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
                    it->second += &r,
                    used = true;
            }

            if (not used)
            if (r.weight < 20
            or  r.kind == "video")
            unused_resources.
                emplace(&r);
        }

        std::unordered_set<res> single_videos;

        for (auto& [s, rr]: course_vocabulary)
        {
            int n = 0;
            res v = 0;
            for (res r: rr)
            if (r->kind == "video") n++, v = r;
            if (n == 1) single_videos.emplace(v);
        }

        for (auto& [s, rr]: course_vocabulary)
        {
            int n = 0;
            for (res r: rr)
            if (r->kind == "video") n++;
            if (n <= 1) continue;

            array<res> aa;
            array<res> vv;
            for (res r: rr)
            if (r->kind == "video")
            vv += r; else
            aa += r;

            vv.erase_if([&single_videos](res r){
            return single_videos.contains(r); });

            if (not vv.empty())
            rr = aa * vv;
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
            bool
            nopixed = entry.opt.internal.contains("pix-");
            auto en = entry.en;
            auto uk = entry.uk;
            auto us = entry.us;
            bool vi = nopixed;

            for (str& s: entry.vocabulary)
            for (res& r: course_vocabulary[s])
            {
                if (r->kind == "video"
                and nopixed) continue;

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

        std::unordered_map<str,
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

        std::unordered_set<str>
            current_vocabulary;

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
                report::audiop += linked(
                html(entry.eng) + "  " +
                dark(entry.pretty_link()),
                     entry.link);

                for (res r: audios)
                report::audiop += 
                dark(html(r->
                     full()));
            }
            if (not videos.empty())
            {
                report::videop += linked(
                html(entry.eng) + "  " +
                dark(entry.pretty_link()),
                     entry.link);

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
            // 's 't 're 'll
            if (eng::list::contractionparts.
                contains(r->title))
                continue;

            if (r->kind == "audio")
                report::audioq +=
                r->full();

            if (r->kind == "video")
                report::videoq +=
                r->full();
        }

        report::save();
    }
}
