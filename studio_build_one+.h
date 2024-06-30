#pragma once
#include "app.h"
namespace studio::one
{
    namespace report
    {
        struct report;
        std::map<str, report*> map;
        struct report
        {
            str name;
            array<str> log;
            report (str name) : name(name) { map[name] = this; }
            void operator += (auto&& s) { log += std::forward<decltype(s)>(s); }
            path file () { return "../data/report/one_"+name+".txt"; }
            void load () { log = sys::optional_text_lines(file()); }
            void save () { sys::write(file(), log); }
        };

        report errors("errors");
        report anoma1("anoma1");
        report anoma2("anoma2");
        report anomal("anomal");
        report duples("duples");
        report audiom("audiom");
        report audiop("audiop");
        report audioq("audioq");
        report videom("videom");
        report videop("videop");
        report videoq("videoq");

        void load () { for (auto& [name, report]: map) report->load(); }
        void save () { for (auto& [name, report]: map) report->save(); }
        void clear() { for (auto& [name, report]: map) report->log.clear(); }
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

    str cliplink (res const& r)
    {
        str s = r->full();
        if (r->options.
            contains("sound"))
            s = "[" + s + "]";

        return linked(
        dark(html(s)),
        "clipboard://: " + r->abstract +
        "file://" + str(r->path));
    }
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
    str link (content::out::entry& e)
    {
        return link(&e);
    }

    void reporting (
        content::out::course const& course,
        hashmap<str, voc> const& course_vocabulary)
    {
        for (auto [s,voc]: course_vocabulary)
        {
            if (voc.entries.size() < 2)
                continue;

            int actual = 0;
            for (ent entry: voc.entries)
            if  (not entry->opt.external.contains("HEAD")
            and  not entry->opt.internal.contains("duple"))
                actual++;

            int duples = 0;
            for (ent entry: voc.entries)
            if  (entry->opt.internal.contains("duple"))
                duples++;

            if (actual == 0 and duples != 0) {
            for (ent entry: voc.entries)
            report::anoma1 += link(entry);
            report::anoma1 += ""; }

            if (actual < 2)
                continue;

            for (ent entry: voc.entries)
            if  (not entry->opt.external.contains("HEAD"))
            report::duples += link(entry);
            report::duples += "";
        }
    }

    struct sensecontrol
    {
        hashmap<str, hashmap<ent, bool>> vocabs;
        hashmap<str, hashmap<res, bool>> audios;
        hashmap<str, hashmap<res, bool>> videos;

        sensecontrol
        (
            array<Ent>& entries,
            array<Res>& resources
        )
        {
            for (auto& entry: entries)
            {
                if (entry.sense == ""
                or  entry.sense == "@")
                    continue;

                for (str s: entry.vocabulary)
                {
                    str dummy = 
                    s.extract_from("@");
                    vocabs[s][&entry] = false;
                }
            }

            for (auto& r: resources)
            {
                if (r.sense == "") continue;
                str abstract = simple(r.
                    abstract);

                str dummy = 
                abstract.extract_from("@");

                if (r.kind == "video"
                or  r.kind == "audio"
                and r.options.contains("sound"))
                videos[abstract][&r] = false; else
                audios[abstract][&r] = false;
            }

            for (auto& entry: entries)
            {
                if (entry.sense == "@") continue;
                if (entry.sense == "")
                {
                    for (str s: entry.vocabulary)
                    if (audios.contains(s)
                    or  videos.contains(s))
                    {
                        report::errors += bold(
                        red("sensless: ")) +
                        link(entry);

                        if (vocabs.contains(s))
                        for (auto [e,_]: vocabs[s]) if (e != &entry)
                        report::errors += link(e);

                        if (audios.contains(s))
                        for (auto [r,_]: audios[s])
                        report::errors += link(r);

                        if (videos.contains(s))
                        for (auto [r,_]: videos[s])
                        report::errors += link(r);
                    }
                }
                else
                {
                    for (str s: entry.vocabulary)
                    {
                        str sense = s.extract_from("@");

                        if (audios.contains(s))
                        for (auto& [r, used]: audios[s])
                        if (r->sense == sense) used = true;

                        if (videos.contains(s))
                        for (auto& [r, used]: videos[s])
                        if (r->sense == sense) used = true;
                    }
                }
            }

            for (auto& r: resources)
            {
                if (r.sense != "") continue;
                str abstract = simple(r.
                    abstract);

                str sense = 
                abstract.extract_from("@");
                str s = abstract;

                bool videolike = false
                or  r.kind == "video"
                or  r.kind == "audio"
                and r.options.contains("sound");

                auto& medios = videolike ?
                    videos : audios;

                if (medios.contains(s)
                or  vocabs.contains(s) and videolike)
                {
                    report::errors += bold(
                    red("sensless: ")) +
                    link(&r);

                    if (vocabs.contains(s))
                    for (auto [e,_]: vocabs[s])
                    report::errors += link(e);

                    if (medios.contains(s))
                    for (auto [r,_]: medios[s])
                    report::errors += link(r);
                }
            }
        }

        // do not report immediately
        // all unused senses of resources,
        // allow some for dictionary sake,
        // report only if resource wasn't used at all

        void report_unused (hashset<res>& unused_resources)
        {
            for (auto X: {&audios, &videos})
            for (auto& [s, x]: *X) for (auto& [R, used]: x)
            if  (not used and unused_resources.contains(R))
            {
                unused_resources.erase(R);

                report::errors += bold(
                red("unused sense: ")) +
                link(R);

                if (vocabs.contains(s))
                for (auto [e,_]: vocabs[s])
                report::errors += link(e);

                if (audios.contains(s))
                for (auto [r,_]: audios[s]) if (r != R)
                report::errors += link(r);

                if (videos.contains(s))
                for (auto [r,_]: videos[s]) if (r != R)
                report::errors += link(r);
            }
        }
    };

    auto all_entries(res r, eng::vocabulary& vocabulary)
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
        return entries;
    }

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
            auto entries = all_entries(r, vocabulary);

            if (r->kind == "audio"
            and entries.size() == 1)
                continue;

            for (str& s: entries)
            s = eng::lowercased(simple(s));
            entries.deduplicate();

            for (str s: entries) if (s.size() >= 2)
            unused_resources_vocabulary[s] += r;
        }

        hashset<str> current_vocabulary;

        for (auto& entry: course.entries)
        {
            if (entry.eng.starts_with(":"))
                continue;

            array<res> audios;
            array<res> videos;

            for (str phrase: entry.vocabulary)
            for (str word: eng::parser::entries(vocabulary, phrase.upto_first("@"), true))
            for (auto forms = eng::forms(word); str form: forms)
            {
                form = eng::lowercased(simple(form));
                
                if (
                current_vocabulary.contains(form)) continue;
                current_vocabulary.emplace(form);

                auto it = unused_resources_vocabulary.find(form);
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
                    for (str x: all_entries(r, vocabulary))
                    if  (x.size() >= 2 // skip "," "!" "?"
                    and  not x.contains(" ") // "see you"
                    and  not current_vocabulary.contains(
                         eng::lowercased(simple(x)))) {
                         well_known = false;
                         break; }

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
                cliplink(r);
            }
            if (not videos.empty())
            {
                report::videop += link(entry);
                for (res r: videos)
                report::videop +=
                cliplink(r);
            }
        }
    }
}
