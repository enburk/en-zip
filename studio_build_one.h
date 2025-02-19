﻿#pragma once
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

        hashmap<res, array<ent>> multientry;
        array<std::pair<ent, array<res>>> multivideo;

        out << dark(bold("ONE: SCAN ENTRIES..."));

        hashset<str> course_vocabulary_forms;
        hashmap<str, voc> course_vocabulary;
        for (auto& entry: course.entries)
        {
            for (str s: entry.vocabulary)
            {
                course_vocabulary[s].
                entries += &entry;

                str ignored_sense =
                s.extract_from("@");

                for (str f: eng::forms(s))
                course_vocabulary_forms.
                    emplace(f);
            }

            if (entry.eng.starts_with(": "))
            {
                str s = entry.eng.from(2);
                str ignored_sense =
                s.extract_from("@");

                if (s.contains(" "))
                    continue;

                s.replace_all(".", "");
                s.replace_all("!", "");
                s.replace_all("?", "");
                s = s.ascii_lowercased();
                for (str f: eng::forms(s))
                course_vocabulary_forms.
                    emplace(f);
            }
            else
            if (entry.eng.contains(" "))
            {
                str ss = entry.eng;
                str ignored_sense =
                ss.extract_from("@");
                ss.replace_all(u8" → ", ", "),
                ss.replace_all(",", "");

                for (str s: ss.split_by(" "))
                for (str f: eng::forms(s))
                course_vocabulary_forms.
                    emplace(f);
            }
        }

        reporting(course,
        course_vocabulary);

        out << dark(bold("ONE: SCAN RESOURCES..."));

        sensecontrol
        sensecontrol(
        course.entries,
        data.resources);

        int more_audio = 0;
        int more_video = 0;

        array<str> sounds;
        hashset<res> unused_resources;
        for (auto& r: data.resources)
        {
            if (r.options.contains("asset"))
                continue;

            str abstract = simple(r.
                abstract);

            if (r.kind == "audio"
            and r.sense == "" and
            not r.options.contains("sound"))
            if (sensecontrol.vocabs.contains(abstract))
            {
                // vocalization fits for any sense,
                // unless there is another one with provided sense,
                // then it's an error and will be reported by sense-control
                for (auto [entry,_]: sensecontrol.vocabs[abstract])
                for (str s: entry->vocabulary)
                {
                    auto it = course_vocabulary.find(s);
                    if (it != course_vocabulary.end())
                        it->second.resources += &r;
                }
                // "@@" ignored by sense-control
                auto it = course_vocabulary.find(abstract+"@@");
                if (it != course_vocabulary.end())
                    it->second.resources += &r;
                // if there is sense-less entry
                // then it's an error and it'll be reported
                // by sense-control
                continue;
            }

            array<str>
            resource_vocabulary;
            resource_vocabulary += abstract;
            {
                auto& w = abstract;
                auto& v = resource_vocabulary;

                if (w.starts_with("a "  )) v += w.from(2); else
                if (w.starts_with("an " )) v += w.from(3); else
                if (w.starts_with("the ")) v += w.from(4); else
                if (w.starts_with("to " )) v += w.from(3); else
                {}
                if (w.starts_with("a "  )) v += str(w.from(2)) + "@@"; else
                if (w.starts_with("an " )) v += str(w.from(3)) + "@@"; else
                if (w.starts_with("the ")) v += str(w.from(4)) + "@@"; else
                if (w.starts_with("to " )) v += str(w.from(3)) + "@@"; else
                {}
                if (w.starts_with("a "  )) v += str(w.from(2)) + "@noun"; else
                if (w.starts_with("an " )) v += str(w.from(3)) + "@noun"; else
                if (w.starts_with("the ")) v += str(w.from(4)) + "@noun"; else
                if (w.starts_with("to " )) v += str(w.from(3)) + "@verb"; else
                {}
            }

            if (r.kind == "audio"
            and r.sense == "" and
            not r.options.contains("sound")
            and abstract.contains("("))
                resource_vocabulary +=
                abstract.debracketed("(",")");

            str
            abstract_ = abstract;
            abstract_.replace_all("_", "");
            if (abstract_!= abstract)
                resource_vocabulary +=
                abstract_;

            if (r.sense == "")
                resource_vocabulary +=
                abstract + "@@";

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
            for (str s: resource_vocabulary)
            {
                auto it = course_vocabulary.find(s);
                if (it != course_vocabulary.end())
                    it->second.resources += &r,
                    used = true;
            }

            if (used
            or  abstract.size() < 2
            or  r.options.contains("=")
            or  r.options.contains("==")
            or  r.options.contains("sic!")
            or  r.options.contains("course-")
            or  course_vocabulary_forms.
                contains(abstract))
                continue;

            if (r.options.contains("="))
            report::anoma2 += cliplink(&r);

            if (r.options.contains("xlam"))
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

            int k =
            false?max<int>() :
            spaces == 0 ? 20 :
            spaces == 1 ? 50 : 200;

            if (true//false
            or  r.weight < k
            or  r.sense != ""
            or  r.kind == "video")
            unused_resources.
                emplace(&r);
            else
            if (r.kind == "audio"
            and abstract.contains(" "))
                more_audio++;

            if (r.kind == "audio"
            and r.options.contains("sound"))
            {
                str s = "../datae\\audiohero {{$audiohero.com}}\\## sound";
                str dir = r.path.parent_path().string();
                if (not dir.starts_with(s)) continue;
                dir = dir.from(s.size());
                sounds += dir + "/" + r.abstract;
            }
        }

        sounds.deduplicate();
        sys::write("../data/sounds.txt",
        sounds);

        sensecontrol.report_unused(unused_resources);

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

            for (str& s: entry.vocabulary)
            for (res& r: course_vocabulary[s].resources)
            {
                if (r->kind == "video"
                and nopixed) continue;

                if (r->kind == "audio"
                and noaudio) continue;

                if (r->kind == "video")
                    videos += r;

                data.one_add(i, r);

                str w = simple(r->abstract);

                if (r->kind == "audio") {
                if (r->options.contains("uk")) fullfill(uk, w);
                if (r->options.contains("us")) fullfill(us, w);
                /*        in any case       */ fullfill(en, w); }

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

        suggestions(course, unused_resources, vocabulary);
        order_check(course, vocabulary);

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

            if (r->audiolike() and
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

        report::audioq.log += bold(blue(
        "+" + str(more_audio) + " more"));

        report::videoq.log += bold(blue(
        "+" + str(more_video) + " more"));

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

        report::save();

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
    }
}
