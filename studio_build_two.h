#pragma once
#include "studia_aux.h"
#include "studio_build_aux.h"
namespace studio::two
{
    void compile (media::out::data& data)
    {
        auto& out = app::logs::report;
        auto& err = app::logs::errors;
        namespace report = studia::aux::report::two;

        out << dark(bold("TWO: SCAN COURSE..."));
        content::out::course course("catalog");
        report::errors += course.errors;
        report::anomal += course.anomal;
        if (not course.errors.empty())
        err << red(bold("TWO ERRORS:")),
        err << course.errors;

        out << dark(bold("TWO: SCAN ENTRIES..."));

        hashset<str> course_vocabulary;
        hashmap<str, voc> course_matches;
        for (int i=0; i < course.entries.size(); i++)
        {
            auto& entry = course.entries[i];

            for (str s: entry.matches)
            course_matches[s].entries += &entry;

            entry.vocabulate(app::vocabulary);
            for (str v: entry.vocabulary)
            course_vocabulary.emplace(v);

            if (entry.eng.starts_with(": "))
                continue;

            for (str w: wrong_words(entry))
            report::errors += red(bold(w +" : ")) + link(entry);
        }

        out << dark(bold("TWO: SCAN RESOURCES..."));

        for (auto& r: data.resources)
        {
            if (r.options.contains("asset")
            or  r.options.contains("noqrop")
            or  r.options.contains("course--"))
                continue;

            if (r.vocal() and
            not r.options.contains("(o)")
            and r.abstract.contains(one_of("()")))
            report::errors += red(bold("(): ")) + link(&r);

            array<str> matches;
            {
                const str& a = r.abstract; matches += a;
                if (a.starts_with("a "  )) matches += str(a.from(2)); else
                if (a.starts_with("an " )) matches += str(a.from(3)); else
                if (a.starts_with("the ")) matches += str(a.from(4)); else
                if (a.starts_with("to " )) matches += str(a.from(3)); else
                {}
            }

            if (r.kind == "video")
            {
                if (r.entries.contains("+"))
                matches +=
                    eng::parser::entries(
                    app::vocabulary, r.title,
                    r.options.contains
                    ("Case"));

                matches += r.entries;
            }

            for (str& s: matches)
            s = s.extract_upto("@");
            matches.deduplicate();

            for (str s: matches)
            if (course_matches.contains(s))
                course_matches[s].resources += &r;
        }

        out << dark(bold("ONE: FULFILL..."));

        hashmap<ent, array<res>> vocals;
        hashmap<ent, array<res>> sounds;
        hashmap<ent, array<res>> videos;
        hashset<res> resources_used;
        hashset<res> resources_single;

        for (Ent& entry: course.entries)
        {
            array<str> all_words;
            for (str ss: entry.en*entry.uk*entry.us)
            for (str s: ss.split_by("|"))
                all_words += s;

            for (str& s: entry.matches)
            for (res& r: course_matches[s].resources)
            {
                bool isahead = entry.opt.external.contains("HEAD");
                bool nopixed = entry.opt.internal.contains("pix-") or isahead;
                bool noaudio = entry.opt.internal.contains("audio-");
                bool nosound = entry.opt.internal.contains("sound-");

                if (r->sound() and nosound) continue;
                if (r->vocal() and noaudio) continue;
                if (r->video() and nopixed) continue;

                if (r->sound() and sound_fits(entry, *r)) sounds[&entry] += r;
                if (r->vocal() and vocal_fits(entry, *r)) vocals[&entry] += r;
                if (r->video() and video_fits(entry, *r)) videos[&entry] += r;

                if (r->vocal()
                    and vocal_will_not_fit(entry, *r))
                    resources_used.emplace(r);

                if (r->vocal()
                and all_words.size() > 1
                and all_words.contains(str(r->abstract).extract_upto("@")))
                    resources_used.emplace(r);
            }

            for (auto medio: {&vocals, &sounds, &videos})
            if ((*medio).contains(&entry)
            and (*medio)[&entry].size() == 1) resources_single.emplace(
                (*medio)[&entry].front());
        }

        int entry_number = 0;
        for (Ent& entry: course.entries)
        {
            for (auto medio: {&vocals, &sounds, &videos})
            {
                if (not (*medio).contains(&entry)) continue;
                auto& mm = (*medio)[&entry];

                auto xx = mm;
                xx.erase_if([](res r){
                return r->options.contains("xlam"); });
                if (not xx.empty()) mm = xx;

                auto yy = mm;
                yy.erase_if([&resources_single](res r){
                return resources_single.contains(r); });
                if (not yy.empty()) mm = yy;

                for (res r: mm)
                data.one_add(entry_number, r),
                resources_used.emplace(r);
            }

            entry_number++;
        }

        out << dark(bold("ONE: CHECK FULFILMENT..."));

        for (Ent& entry: course.entries)
        {
            if (entry.matches.empty()) continue;

            bool isahead = entry.opt.external.contains("HEAD");
            bool nopixed = entry.opt.internal.contains("pix-") or isahead;
            bool nopixal = entry.opt.internal.contains("pix?");
            bool noaudio = entry.opt.internal.contains("audio-");
            bool nosound = entry.opt.internal.contains("sound-");
            bool soundio = entry.opt.external.contains("SOUND");

            bool vocal_ok = not vocals[&entry].empty() or noaudio or soundio;
            bool sound_ok = not sounds[&entry].empty() or nosound or not soundio;
            bool video_ok = not videos[&entry].empty() or nopixed or nopixal or
            entry.eng.contains(str(u8"→")) or
            entry.eng.starts_with(":") or
            entry.eng == "";

            str s = html(entry.eng);
            if (not vocal_ok) report::audiom += linked(s, entry.link);
            if (not sound_ok) report::audiom += linked(s, entry.link) + red(bold(" sound"));

            if (vocal_ok)
                continue;

            auto fullfill = [](array<str>& units, str word)
            {
                units.erase_if([word](str& unit)
                {
                    for (str s: unit.split_by("|"))
                    if (s == word) return true;
                    return false;
                });
            };

            array<res> rr;
            for (str& s: entry.matches)
            for (res& r: course_matches[s].resources)
            if (r->vocal()) rr += r;

            auto ens = entry.en;
            auto uks = entry.uk;
            auto uss = entry.us;

            for (res r: rr)
            if  (not r->options.contains("xlam"))
            {
                bool uk = r->options.contains("uk");
                bool us = r->options.contains("us");
                bool en = true;

                if (uk) fullfill(uks, r->abstract);
                if (us) fullfill(uss, r->abstract);
                if (en) fullfill(ens, r->abstract);
            }

            data.needed_en += ens;
            data.needed_us += uss;
            data.needed_uk += uks;

            array<array<res>> list;

            auto fill = [&list, &rr](array<str>& units, str lang)
            {
                for (str ss: units)
                {
                    list += array<res>{};
                    for (str s: ss.split_by("|")) for (res r: rr)
                    if (not r->options.contains("xlam"))
                    if (r->options.contains(lang) or lang == "")
                    if (r->abstract == s) list.back() += r;
                    if (list.back().empty()) return false;
                }
                return true;
            };

            if (not fill(entry.en, "")
            or  not fill(entry.uk, "uk")
            or  not fill(entry.us, "us"))
                continue;

            int nn = 0;
            auto combine = [&list, &nn, &entry]
            (this auto const& combine, array<path> pp, int n) -> void
            {
                if (n >= list.size())
                {
                    path dst = "../data/combine/";
                    dst += entry.abstract.upto_first("@") + " ### " + str(nn++) + ".wav";
                    media::audio::combine(pp, dst, 0.1, 0.0, 0.0, false);
                    return;
                }
                for (res r: list[n])
                combine(pp + r->path, n+1);
            };
            combine({}, 0);
        }

        out << dark(bold("ONE: MAKE REPORTS..."));

        hashset<res> unused_resources;
        for (auto& r: data.resources)
        {
            if (resources_used.contains(&r)
            or  r.abstract.size() < 2
            or  r.options.contains("=")
            or  r.options.contains("==")
            or  r.options.contains("sic!")
            or  r.options.contains("xlam")
            or  r.options.contains("noqrop")
            or  r.options.contains("course-")
            or  course_vocabulary.
                contains(r.abstract))
                continue;

            int spaces = 0;
            for (char c: r.abstract)
            if  (c == ' ') spaces++;

            if (spaces == 0 and r.vocal())
                continue;

            if (spaces == 2 and r.vocal())
            if (r.abstract.starts_with("to be "))
                continue;

            unused_resources.emplace(&r);
        }

        sensecontrol
        sensecontrol(
        course.entries,
        data.resources);

        sensecontrol.report_unused(unused_resources);

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

            if (r->vocal())
                report::audioq +=
                cliplink(r, course_vocabulary);

            if (r->video())
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

        hashmap<res, array<ent>> multientry;
        array<std::pair<ent, array<res>>> multivideo;
        for (auto [i, entry]: enumerate(course.entries))
        {
            for (res r: videos[&entry])
                multientry[r] +=
                &course.entries[i];

            if (videos[&entry].size() >= 2)
            multivideo += std::make_pair(
                &course.entries[i],
                videos[&entry]);
        }

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
            app::vocabulary, e.word, false);

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

        // combine samples
        array<path> src =
        sys::files("../datae_sample/combine");
        path dst = "../datae_sample/combined.wav";
        if (not src.empty()) media::audio::combine(src, dst, 0.1, 0.0, 0.0, true);
    }
}
