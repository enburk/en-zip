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

        out << dark(bold("ONE: SCAN ENTRIES..."));

        array<str> needed_en;
        array<str> needed_us;
        array<str> needed_uk;

        struct nln
        {
            int n = 0;
            int last = 0;
            void add (int l)
            {
                if (last = 0 or last+10 > l)
                if (n < 6) n++;
                last = l;
            }
        };

        hashset<str> course_vocabulary;
        hashmap<str, nln> course_ens;
        hashmap<str, nln> course_uss;
        hashmap<str, nln> course_uks;
        hashmap<str, voc> course_matches;
        for (int i=0; i < course.entries.size(); i++)
        {
            auto& entry = course.entries[i];

            for (str s: entry.matches)
            course_matches[s].entries += &entry;

            entry.vocabulate(vocabulary);
            for (str v: entry.vocabulary)
            course_vocabulary.emplace(v);

            if (entry.eng.starts_with(": "))
                continue;

            if  (not entry.opt.internal.contains("sic!"))
            for (str ss: entry.en*entry.us*entry.uk)
            for (str s: ss.split_by("|"))
            {
                s.erase_all('(');
                s.erase_all(',');
                s.erase_all(';');
                s.erase_all('.');
                s.erase_all('?');
                s.erase_all('!');
                s.erase_all(')');
                s.replace_all("/", " ");
                s.replace_all(u8"“", "");
                s.replace_all(u8"”", "");
                for (str w: s.split_by(" "))
                if (w == eng::lowercased(w)
                and not vocabulary.contains(w)
                and not vocabulary.contains(s))
                report::errors += red(bold(w +" : ")) + link(entry);
            }
            
            for (str ss: entry.en) for (str s: ss.split_by("|")) course_ens[s].add(i);
            for (str ss: entry.us) for (str s: ss.split_by("|")) course_uss[s].add(i);
            for (str ss: entry.uk) for (str s: ss.split_by("|")) course_uks[s].add(i);
        }

        std::erase_if(course_ens, [](auto pair){ return pair.second.n < 2; });
        std::erase_if(course_uss, [](auto pair){ return pair.second.n < 2; });
        std::erase_if(course_uks, [](auto pair){ return pair.second.n < 2; });

        report_duples(course);

        report_missing_words(course_vocabulary, data.resources, vocabulary);

        report_shortenings(data.resources);

        report_long_sounds(data);

        out << dark(bold("ONE: SCAN RESOURCES..."));

        for (auto& r: data.resources)
        {
            if (r.options.contains("asset")
            or  r.options.contains("noqrop"))
                continue;

            if (r.vocal() and
            not r.options.contains("(o)")
            and r.abstract.contains(one_of("()")))
            report::errors += red(bold("(): ")) + link(&r);

            if (r.options.contains("="))
            report::anoma2 += cliplink(&r) + red(bold(" ## ="));

            if (r.vocal())
            {
                str& a  = r.abstract;
                bool uk = r.options.contains("uk");
                bool us = r.options.contains("us");
                bool en = true;

                if (uk and course_uks.contains(a)) course_uks[a].n--;
                if (us and course_uss.contains(a)) course_uss[a].n--;
                if (en and course_ens.contains(a)) course_ens[a].n--;
            }

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
                    vocabulary, r.title,
                    r.options.contains
                    ("Case"));

                matches += r.entries;
            }

            for (str& s: matches)
            s = s.extract_upto("@");
            matches.deduplicate();

            for (str s: matches)
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

                if (r->sound() and entry.audio_fits(r->abstract))  sounds[&entry] += r;
                if (r->vocal() and entry.audio_fits(r->abstract))  vocals[&entry] += r;
                if (r->video() and entry.video_fits(r->Entries())) videos[&entry] += r;
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
                xx.erase_if([&resources_single](res r){
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
            bool noaudio = entry.opt.internal.contains("audio-");
            bool nosound = entry.opt.internal.contains("sound-");
            bool soundio = entry.opt.external.contains("SOUND");

            if (noaudio) report::anoma2 += link(entry) + red(bold(" audio-"));
            if (nosound) report::anoma2 += link(entry) + red(bold(" sound-"));

            bool vocal_ok = not vocals[&entry].empty() or noaudio or soundio;
            bool sound_ok = not sounds[&entry].empty() or nosound or not soundio;
            bool video_ok = not videos[&entry].empty() or nopixed or
            entry.eng.contains(str(u8"→")) or
            entry.eng.starts_with(":") or
            entry.eng == "";

            str s = html(entry.eng);
            if (not vocal_ok) report::audiom += linked(s, entry.link);
            if (not sound_ok) report::audiom += linked(s, entry.link) + red(bold(" sound"));
            if (not video_ok) report::videom += linked(s, entry.link);

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

            needed_en += ens;
            needed_us += uss;
            needed_uk += uks;

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
            or  r.options.contains("course-")
            or  course_vocabulary.
                contains(r.abstract))
                continue;

            unused_resources.emplace(&r);
        }

        sensecontrol
        sensecontrol(
        course.entries,
        data.resources);

        sensecontrol.report_unused(unused_resources);
        suggestions(course, unused_resources, vocabulary);
        order_check(course, vocabulary);

        array<str>  unused_sounds;
        for (res r: unused_resources) if (r->sound())
        {
            str s = "../datae\\audiohero {{$audiohero.com}}\\## sound";
            str dir = r->path.parent_path().string();
            if (not dir.starts_with(s)) continue;
            dir = dir.from(s.size());
            unused_sounds += dir + "/" + r->abstract;
            report::audioq += cliplink(r);
        }
        unused_sounds.deduplicate();
        sys::write("../data/sounds.txt",
        unused_sounds);

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
                cliplink(r);

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

        needed_en.stable_deduplicate(); needed_en += "";
        needed_us.stable_deduplicate(); needed_us += "";
        needed_uk.stable_deduplicate(); needed_uk += "";

        for (auto [s,x]: course_ens) if (x.n > 0) needed_en.try_emplace(s);
        for (auto [s,x]: course_uss) if (x.n > 0) needed_us.try_emplace(s);
        for (auto [s,x]: course_uks) if (x.n > 0) needed_uk.try_emplace(s);

        sys::write("../data/needed_en.txt", needed_en);
        sys::write("../data/needed_us.txt", needed_us);
        sys::write("../data/needed_uk.txt", needed_uk);

        // combine samples
        array<path> src =
        sys::files("../datae_sample/combine");
        path dst = "../datae_sample/combined.wav";
        if (not src.empty()) media::audio::combine(src, dst, 0.1, 0.0, 0.0, true);

        ///////////////
        report::save();
    }
}
