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

                str sense =
                s.extract_from("@");

                for (str f: forms(s))
                course_vocabulary_forms.
                    emplace(f);
            }

            if (entry.eng.starts_with(": "))
            {
                if (entry.eng.contains(" "))
                    continue;

                str s = entry.eng;
                s.replace_all(".", "");
                s.replace_all("!", "");
                s.replace_all("?", "");
                s = s.ascii_lowercased();
                course_vocabulary_forms.
                    emplace(s);
            }
            else
            if (entry.eng.contains(" "))
            {
                str ss = entry.eng;
                str sense = ss.extract_from("@");
                ss.replace_all(u8" → ", ", "),
                ss.replace_all(",", "");

                for (str s: ss.split_by(" "))
                course_vocabulary_forms.
                    emplace(s);

                for (str s: ss.split_by(" "))
                for (str f: forms(s))
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

        hashset<res> unused_resources;
        for (auto& r: data.resources)
        {
            str abstract = simple(r.
                abstract);

            if (r.kind == "audio"
            and r.sense == "" and
            not r.options.contains("sound"))
            // vocalization fits for any sense,
            // if there is another one with provided sense
            // then it's an error and will be reported by sense-control
            if (sensecontrol.vocabs.contains(abstract))
            {
                for (auto [entry,_]: sensecontrol.vocabs[abstract])
                for (str s: entry->vocabulary)
                {
                    auto it = course_vocabulary.find(s);
                    if (it != course_vocabulary.end())
                        it->second.resources += &r;
                }
                // if there is sense-less entry
                // then it's an error and will be reported
                // by sense-control
                continue;
            }

            array<str>
            resource_vocabulary;
            resource_vocabulary += abstract;
            {
                auto& w = abstract;
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
            for (str s: resource_vocabulary)
            {
                auto it = course_vocabulary.find(s);
                if (it != course_vocabulary.end())
                    it->second.resources += &r,
                    used = true;
            }

            if (used
            or  abstract.size() < 2
            or  course_vocabulary_forms.
                contains(abstract))
                continue;

            int spaces = 0;
            for (char c: abstract)
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
            bool video_ok = nopixed;

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
                link(r);

            if (r->kind == "video")
                report::videoq +=
                link(r);
        }

        if (int nn = 500, n = 
        report::audiom.log.size(); n > 2*nn)
        report::audiom.log.resize(nn),
        report::audiom.log += bold(blue(
        "+" + str(n-nn) + " more"));

        if (int nn = 500, n = 
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
        if (true) sys::out::file("../data/course_searchmap.dat") << course.searchmap;
    }
}
