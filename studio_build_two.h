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

        out << dark(bold("TWO: FULFILL..."));

        hashmap<ent, array<res>> vocals;
        hashmap<ent, array<res>> sounds;
        hashmap<ent, array<res>> videos;
        hashset<res> resources_single;

        for (Ent& entry: course.entries)
        {
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
            }

            for (auto medio: {&vocals, &sounds, &videos})
            if ((*medio).contains(&entry)
            and (*medio)[&entry].size() == 1) resources_single.emplace(
                (*medio)[&entry].front());
        }

        for (auto [i, entry]: enumerate(course.entries))
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
                data.two_add(i, r);
            }
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

        out << dark(bold("TWO: MAKE REPORTS..."));

        sensecontrol
        sensecontrol(
        course.entries,
        data.resources,
        report::errors.log);

        // hashmap<res, array<ent>> multientry;
        // array<std::pair<ent, array<res>>> multivideo;
        // for (auto [i, entry]: enumerate(course.entries))
        // {
        //     for (res r: videos[&entry])
        //         multientry[r] +=
        //         &course.entries[i];
        // 
        //     if (videos[&entry].size() >= 2)
        //     multivideo += std::make_pair(
        //         &course.entries[i],
        //         videos[&entry]);
        // }
        // 
        // if (true)
        // generate_debug_theme(
        //     course,
        //     multivideo,
        //     multientry,
        //     data);

        if (true) sys::out::file("../data/catalog.dat") << course.root;
        if (true) sys::out::file("../data/catalog_entries.dat") << course.entries;
        if (true) sys::out::file("../data/catalog_searchmap.dat") << course.searchmap;
    }
}
