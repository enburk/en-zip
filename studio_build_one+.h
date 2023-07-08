#pragma once
#include "app.h"
namespace studio::one
{
    namespace report
    {
        array<str> errors;
        array<str> anomal;
        array<str> duples;
        array<str> audiom, audiop, audioq;
        array<str> videom, videop, videoq;
        void load ()
        {
            std::filesystem::path dir = "../data/report";
            errors = sys::in::optional_text_lines(dir/"one_errors.txt");
        //  anomal = sys::in::optional_text_lines(dir/"one_anomal.txt");
        //  duples = sys::in::optional_text_lines(dir/"one_duples.txt");
        //  audiom = sys::in::optional_text_lines(dir/"one_audiom.txt");
        //  videom = sys::in::optional_text_lines(dir/"one_videom.txt");
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
            sys::out::write(dir/"one_duples.txt", duples);
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
            duples.clear();
            audiom.clear();
            videom.clear();
            audiop.clear();
            videop.clear();
            audioq.clear();
            videoq.clear();
        }
    }

    using Res = media::resource;
    using res = media::resource*;
    using Ent = content::out::entry;
    using ent = content::out::entry*;

    struct voc
    {
        array<ent> entries;
        array<res> resources;
    };

    str simple (str s)
    {
        s.replace_all(mdash, "---");
        s.replace_all(ndash, "--");
        s.replace_all(u8"’", "'");
        return s;
    };

    str link (res const& r)
    {
        str s = r->full();
        if (r->options.
            contains("sound"))
            s = "[" + s + "]";

        return linked(
        dark(html(s)),
        "file://" + str(r->path));
    }
    str link (ent const& entry)
    {
        return linked(
        html(entry->eng) + "  " +
        dark(entry->pretty_link()),
        "one://" + entry->link);
    }
    str link (content::out::entry& e) {
        return link(&e); }


    generator<str> forms (str s)
    {
        if (s.size() < 3
        or s.contains(" "))
            co_return;

        for (str form: {"s", "ing", "ed"})
        {
            str f = eng::form(s, form);
            if (f != s) co_yield f;

            if (s.size() - form.size() < 3)
                continue;

            str b = eng::backform(s, form);
            if (b != s)  co_yield b;
        }
    }

    struct sensecontrol
    {
        hashmap<str, hashmap<ent, bool>> entrification;
        hashmap<str, hashmap<res, bool>> pronunciation;
        hashmap<str, hashmap<res, bool>> visualisation;

        sensecontrol
        (
            array<Ent>& entries,
            array<Res>& resources
        )
        {
            for (auto& entry: entries) if (entry.sense != "")
            for (str s: entry.vocabulary)
            {
                str sense = s.extract_from("@");
                entrification[s][&entry] = false;
            }

            for (auto& r: resources)
            {
                if (r.sense == "") continue;
                str abstract = simple(r.
                    abstract);

                str sense = abstract.
                    extract_from("@");

                if (r.kind == "video"
                or  r.kind == "audio"
                and r.options.contains("sound"))
                visualisation[abstract][&r] = false; else
                pronunciation[abstract][&r] = false;
            }

            for (auto& entry: entries)
            {
                if (entry.sense == "")
                {
                    for (str s: entry.vocabulary)
                    if (pronunciation.contains(s)
                    or  visualisation.contains(s))
                    {
                        report::errors += bold(
                        red("sensless: ")) +
                        link(entry);

                        if (entrification.contains(s))
                        for (auto [e,_]: entrification[s]) if (e != &entry)
                        report::errors += link(e);

                        if (pronunciation.contains(s))
                        for (auto [r,_]: pronunciation[s])
                        report::errors += link(r);

                        if (visualisation.contains(s))
                        for (auto [r,_]: visualisation[s])
                        report::errors += link(r);
                    }
                }
                else
                {
                    for (str s: entry.vocabulary)
                    {
                        str sense = s.extract_from("@");

                        if (pronunciation.contains(s))
                        for (auto& [r, ok]: pronunciation[s])
                        if (r->sense == sense) ok = true;

                        if (visualisation.contains(s))
                        for (auto& [r, ok]: visualisation[s])
                        if (r->sense == sense) ok = true;
                    }
                }
            }
        }

        void report_unused (hashset<res>& unused_resources)
        {
            for (auto X: {&pronunciation, &visualisation})
            for (auto& [s, x]: *X) for (auto& [R, ok]: x)
            if  (not ok and unused_resources.contains(R))
            {
                unused_resources.erase(R);

                report::errors += bold(
                red("unused sense: ")) +
                link(R);

                if (entrification.contains(s))
                for (auto [e,_]: entrification[s])
                report::errors += link(e);

                if (pronunciation.contains(s))
                for (auto [r,_]: pronunciation[s]) if (r != R)
                report::errors += link(r);

                if (visualisation.contains(s))
                for (auto [r,_]: visualisation[s]) if (r != R)
                report::errors += link(r);
            }
        }
    };

    void suggestions
    (
        content::out::course& course,
        hashset<res>& unused_resources,
        eng::vocabulary& vocabulary
    )
    {
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
                link(r);
            }
            if (not videos.empty())
            {
                report::videop += link(entry);
                for (res r: videos)
                report::videop +=
                link(r);
            }
        }
    }
}
