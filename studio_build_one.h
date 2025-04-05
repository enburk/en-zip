#pragma once
#include "studio_build_one++.h"
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

        report::clear();
        report::errors += course.errors;
        report::anomal += course.anomal;

        if (not report::errors.log.empty()) {
        err << red(bold("ONE ERRORS:"));
        err << report::errors.log; }

        array<str> needed_en;
        array<str> needed_us;
        array<str> needed_uk;

        hashmap<res, array<ent>> multientry;
        array<std::pair<ent, array<res>>> multivideo;

        out << dark(bold("ONE: SCAN ENTRIES..."));

        hashset<str> course_vocabulary;
        hashmap<str, int> course_ens;
        hashmap<str, int> course_uss;
        hashmap<str, int> course_uks;
        hashmap<str, voc> course_matches;
        for (auto& entry: course.entries)
        {
            for (str s: entry.matches)
            course_matches[s].entries += &entry;

            entry.vocabulate(vocabulary);
            for (str v: entry.vocabulary)
            course_vocabulary.emplace(v);

            if (entry.eng.starts_with(": "))
                continue;
            
            for (str s: entry.en) course_ens[s]++;
            for (str s: entry.us) course_uss[s]++;
            for (str s: entry.uk) course_uks[s]++;
        }
        for (auto& [s,n]: course_ens) n = min(n, 6);
        for (auto& [s,n]: course_uss) n = min(n, 6);
        for (auto& [s,n]: course_uks) n = min(n, 6);

        reporting(course, course_matches);

        report_missing_words(course_vocabulary, data.resources, vocabulary);

        report_shortenings(data.resources);

        report_long_sounds(data);

        out << dark(bold("ONE: SCAN RESOURCES..."));

        sensecontrol
        sensecontrol(
        course.entries,
        data.resources);

        auto course_matches_add = [&course_matches](str s, Res& r)
        {
            auto it = course_matches.find(s);
            if (it == course_matches.end()) return false;
            it->second.resources += &r; return true;
        };

        hashset<res> unused_resources;
        for (auto& r: data.resources)
        {
            str abstract = r.
                abstract;

            if (abstract.contains(one_of("()")))
            {
                abstract.debracket("(",")");
                if (not r.options.contains("(o)"))
                report::errors += red(bold("():")) + link(&r);
            }

            if (r.sense == "" and
            not sensecontrol.videolike(r) // vocal
            and sensecontrol.senses.contains(abstract))
            {
                for (str s: sensecontrol.senses[abstract])
                course_matches_add(s, r);
                continue;
            }

            if (course_matches_add(abstract, r))
                continue;

            array<str> matches;
            {
                const auto& a = abstract;
                if (a.starts_with("a "  )) matches += str(a.from(2)) + "@noun"; else
                if (a.starts_with("an " )) matches += str(a.from(3)) + "@noun"; else
                if (a.starts_with("the ")) matches += str(a.from(4)) + "@noun"; else
                if (a.starts_with("to " )) matches += str(a.from(3)) + "@verb"; else
                {}
            }

            if (r.kind == "video")
            {
                if (r.options.contains("asset")
                or  r.options.contains("noqrop"))
                    continue;

                if (r.entries.contains("+"))
                matches +=
                    eng::parser::entries(
                    vocabulary, r.title,
                    r.options.contains
                    ("Case"));

                matches += r.entries;

                const auto& a = abstract;
                if (a.starts_with("a "  )) matches += a.from(2); else
                if (a.starts_with("an " )) matches += a.from(3); else
                if (a.starts_with("the ")) matches += a.from(4); else
                if (a.starts_with("to " )) matches += a.from(3); else
                {}

                array<str> senseless;
                for (str s: matches)
                if (s.contains("@")) senseless += s.extract_upto("@");
                matches += senseless;
                matches.deduplicate();
            }

            bool used = false;
            for (str s: matches)
            if (course_matches_add(s, r))
                 used = true;

            if (r.options.contains("="))
            report::anoma2 += cliplink(&r);

            if (used
            or  abstract.size() < 2
            or  r.options.contains("=")
            or  r.options.contains("==")
            or  r.options.contains("sic!")
            or  r.options.contains("xlam")
            or  r.options.contains("course-")
            or  course_vocabulary.
                contains(abstract))
                continue;

            // for unused resources

            int spaces = 0;
            for (char c: abstract)
            if  (c == ' ')
                spaces++;

            if (spaces == 1)
            if (abstract.starts_with("a "  )
            or  abstract.starts_with("an " )
            or  abstract.starts_with("the ")
            or  abstract.starts_with("to " ))
                continue;

            if (spaces == 2)
            if (abstract.starts_with("to be "))
                continue;

            unused_resources.emplace(&r);
        }

        std::unordered_set<res> single_videos;

        for (auto& [s,voc]: course_matches)
        {
            int n = 0;
            res v = 0;
            for (res r: voc.resources)
            if (r->kind == "video") n++, v = r;
            if (n == 1) single_videos.emplace(v);
        }

        for (auto& [s,voc]: course_matches)
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

        auto fullfill = [](array<str>& units, str word, hashmap<str, int>& nn)
        {
            units.erase_if([word, &nn](str& unit)
            {
                for (str s: unit.split_by("|"))
                if (s == word and --nn[s] <= 0) return true;
                return false;
            });
        };

        for (auto [i, entry]: enumerate(course.entries))
        {
            bool isahead = entry.opt.external.contains("HEAD");
            bool nopixed = entry.opt.internal.contains("pix-") or isahead;
            bool noaudio = entry.opt.internal.contains("audio-");
            bool soundio = entry.opt.external.contains("SOUND");

            auto en = noaudio or soundio? array<str>{} : entry.en;
            auto uk = noaudio or soundio? array<str>{} : entry.uk;
            auto us = noaudio or soundio? array<str>{} : entry.us;

            bool sound_ok = not soundio;
            bool video_ok = nopixed or
            entry.eng.contains(str(u8"→")) or
            entry.eng.starts_with(":") or
            entry.eng == "";

            array<res> videos;

            for (str& s: entry.matches)
            for (res& r: course_matches[s].resources)
            {
                if (r->kind == "video"
                and nopixed) continue;

                if (r->kind == "audio"
                and noaudio) continue;

                if (r->kind == "video")
                    videos += r;

                data.one_add(i, r);

                str w = simple(r->abstract);

                if (r->kind == "audio" and not r->options.contains("xlam")) {
                if (r->options.contains("uk")) fullfill(uk, w, course_uks);
                if (r->options.contains("us")) fullfill(us, w, course_uss);
                /*        in any case       */ fullfill(en, w, course_ens); }

                if (r->kind == "audio"
                and r->options.contains("sound"))
                    sound_ok = true;

                if (r->kind == "video")
                    video_ok = true;
            }

            str s = html(entry.eng);
            str snd = red(bold(" sound "));
            str ens = red(bold(" en: " + html(str(en, ", "))));
            str uks = red(bold(" uk: " + html(str(uk, ", "))));
            str uss = red(bold(" us: " + html(str(us, ", "))));
            if (not en.empty()) report::audiom += linked(s + ens, entry.link);
            if (not uk.empty()) report::audiom += linked(s + uks, entry.link);
            if (not us.empty()) report::audiom += linked(s + uss, entry.link);
            if (not sound_ok  ) report::audiom += linked(s + snd, entry.link);
            if (not video_ok  ) report::videom += linked(s + " ", entry.link);

            for (str s: en) needed_en += s.extract_upto("@");
            for (str s: us) needed_us += s.extract_upto("@");
            for (str s: uk) needed_uk += s.extract_upto("@");

            videos.deduplicate();
            for (res r: videos)
                multientry[r] +=
                &course.entries[i];

            if (videos.size() >= 2)
            multivideo += std::make_pair(
                &course.entries[i],
                std::move(videos));
        }

        out << dark(bold("ONE: MAKE REPORTS..."));

        sensecontrol.report_unused(unused_resources);
        suggestions(course, unused_resources, vocabulary);
        order_check(course, vocabulary);

        array<str> sounds;
        for (res r:  unused_resources)
        {
            if (r->kind == "audio"
            and r->options.contains("sound"))
            {
                str s = "../datae\\audiohero {{$audiohero.com}}\\## sound";
                str dir = r->path.parent_path().string();
                if (not dir.starts_with(s)) continue;
                dir = dir.from(s.size());
                sounds += dir + "/" + r->abstract;
            }
        }
        sounds.deduplicate();
        sys::write("../data/sounds.txt",
        sounds);

        std::map<str, array<res>> words;
        std::map<int, array<res>> Words;

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

            if (r->kind == "audio" and
            not s.contains(" ")) {
                words[s] += r;
                continue; }

            if (r->kind == "audio")
                report::audioq +=
                cliplink(r);

            if (r->kind == "video")
                report::videoq +=
                cliplink(r);
        }

        for (auto& [s, rr] : words) if (rr.size() > 1) Words[rr.size()] += rr;
        for (auto& [n, rr] : Words) for (auto r: rr)
        report::audioq += cliplink(r);

        report::audioq.log += bold(blue(
        "total: " + str(report::audioq.log.size())));

        if (int nn = 500, n = 
        report::audiom.log.size(); n > 2*nn)
        report::audiom.log.resize(nn),
        report::audiom.log += bold(blue(
        "+" + str(n-nn) + " more"));

        if (int nn = 2000, n = 
        report::videom.log.size(); n > 2*nn)
        report::videom.log.resize(nn),
        report::videom.log += bold(blue(
        "+" + str(n-nn) + " more"));

        if (true)
        generate_debug_theme(
            course,
            multivideo,
            multientry,
            data);

        if (true) sys::out::file("../data/course.dat") << course.root;
        if (true) sys::out::file("../data/course_entries.dat") << course.entries;
        if (true) sys::out::file("../data/course_searchmap_title.dat") << course.searchmap;

        using search_entry =
        content::out::course::search_entry;
        array<search_entry> searchmap;
        for (auto& e: course.searchmap)
        {
            auto words =
            eng::parser::entries(
            vocabulary, e.word, false);

            words += e.word.split_by(" "); // ursa major
            words.erase_if([](str const& s){ return s.size() < 2; });
            words.deduplicate();

            for (str w: words)
            searchmap +=
            search_entry(
            w, e.entry, e.link);
        }

        std::ranges::stable_sort(searchmap);
        auto r = std::ranges::unique(searchmap);
        searchmap.erase(r.begin(), r.end());

        if (true) sys::out::file("../data/course_searchmap_words.dat") << searchmap;

        needed_en.stable_deduplicate(); sys::write("../data/needed_en.txt", needed_en);
        needed_us.stable_deduplicate(); sys::write("../data/needed_us.txt", needed_us);
        needed_uk.stable_deduplicate(); sys::write("../data/needed_uk.txt", needed_uk);

        int needed_nn = 1000;
        int step = needed_en.size()/needed_nn;
        if (step == 0) step = 1;
        array<str> needed; needed.reserve(needed_nn);
        for (int i=0; i<needed_en.size(); i += step) needed += needed_en[i];
        sys::write("../data/needed.txt", needed);

        // combine samples
        array<path> src =
        sys::files("../datae_sample/combine");
        path dst = "../datae_sample/combined.wav";
        if (not src.empty()) media::audio::combine(src, dst, 0.1, 0.0, 0.0, true);

        ///////////////
        report::save();
    }
}
