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
        report orders("orders");
        report audiom("audiom");
        report audiop("audiop");
        report audioq("audioq");
        report videom("videom");
        report videop("videop");
        report videoq("videoq");
        report wordsm("wordsm");

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

    str cliplink (res const& r)
    {
        str s = r->full();
        if (r->options.contains("sound"))
            s = "[" + s + "]";

        str abstract = r->abstract;
        if (r->options.contains("sound"))
            abstract += " # SOUND";

        return linked(
        dark(html(s)),
        "clipboard://: " + abstract +
        "file://" + str(r->path));
    }
    str link (media::resource const* r)
    {
        str s = r->full();
        if (r->options.contains("sound"))
            s = "[" + s + "]";

        return linked(
        dark(html(s)),
        "file://" + str(r->path));
    }
    str link (content::out::entry const* entry)
    {
        return linked(
        html(entry->eng) + "  " +
        dark(entry->pretty_link()),
        "one://" + entry->link);
    }
    str link (content::out::entry const& e)
    {
        return link(&e);
    }

    void report_duples (content::out::course& course)
    {
        hashmap<str, ent> entries;

        for (auto& e: course.entries)
        {
            str s = e.abstract + "@" + e.sense;
            auto it = entries.find(s);
            if (it == entries.end())
            {
                entries[s] = &e;
                if (e.opt.internal.contains("duple"))
                report::duples += red(bold("wrong duple:")),
                report::duples += link(e),
                report::duples += "";
            }
            else
            {
                if (not e.opt.internal.contains("duple"))
                report::duples += link(it->second),
                report::duples += link(e),
                report::duples += "";
            }
        }
    }

    void report_missing_words(
        hashset<str> const& course_vocabulary,
        array<media::resource> const& resources)
    {
        hashmap<str, int> resources_vocabulary;

        for (auto& r: resources)
        {
            if (r.kind == "audio")
            for (str s: eng::parser::entries(app::vocabulary, simple(r.title), false))
            if (str(s.upto(1)) != str(s.upto(1)).capitalized()
            and not course_vocabulary.contains(s))
                resources_vocabulary[s]++;
        }

        auto doubt_it = [](str s)
        {
            return false;
        };
        auto skip_it = [](str s)
        {
            const hashset<str> reject =
            {
                "like to", "is all", "in that", "comes from", "for that"
            };

            return false
            or s.size() <= 2
            or s.ends_with("'")
            or s.ends_with("-")
            or s.ends_with(".")
            or eng::list::contractionparts.contains(s)
            or reject.contains(s);
        };

        std::multimap<int,str> resources_vocabulary_sorted;
        std::multimap<int,str> resources_vocabulary_doubted;
        
        for (auto [s, n]: resources_vocabulary) if (n >= 2)
        if (skip_it(s)) {;} else if (doubt_it(s))
        resources_vocabulary_doubted.emplace(n, s); else
        resources_vocabulary_sorted .emplace(n, s);
        
        for (auto [n, s]: resources_vocabulary_sorted)
        report::wordsm.log += s + " (" + str(n) + ")";
        report::wordsm.log += "---------------------";
        
        for (auto [n, s]: resources_vocabulary_doubted)
        report::wordsm.log += s + " (" + str(n) + ")";
        report::wordsm.log += bold(blue(str(
        report::wordsm.log.size()) + " total"));
    }

    void report_shortenings(array<media::resource>& resources)
    {
        array<str> shortenings;

        for (auto& r: resources)
        {
            if (r.kind == "audio"
            and r.abstract != r.title)
            {
                str a = simple(r.abstract);
                str t = simple(r.title);
                a.extract_from("@");
                t.extract_from("@");
                a.replace_all("_", "");
                t.replace_all("_", "");
                t.replace_all("\n", "<br>");
                if (a != t)
                shortenings += a,
                shortenings += t;
            }
        }

        sys::write("../data/shortenings.txt", shortenings);
    }

    void report_long_sounds(media::out::data& data)
    {
        bool sounds_lengths_update = false;
        array<str> sounds_lengths = sys::optional_text_lines(
          "../data/sounds_lengths.txt");

        hashmap<str, int> sounds_lengths_map;
        for (str s: sounds_lengths) {
            str sec = s.extract_upto(" "); if(sec != "")
            sounds_lengths_map[s] = std::stoi(sec); }

        for (auto& r: data.resources)
        if (r.kind == "audio"
        and r.options.contains("sound") and
        not r.options.contains("long"))
        {
            int duration = 0;
            auto it = sounds_lengths_map.find(path2str(r.path));
            if (it == sounds_lengths_map.end())
            {
                auto Location = data.storage.add(r,1);

                auto source = [](int source){
                    return "../data/media/storage." +
                    std::to_string(source) + ".dat"; };

                auto bytes = sys::bytes(source(
                    Location.location.source),
                    Location.location.offset,
                    Location.location.length);

                sys::audio::player audio;
                sys::audio::decoder decoder(bytes);

                audio.load(
                decoder.output,
                decoder.channels,
                decoder.samples,
                decoder.bps);

                duration = int(audio.duration);

                sounds_lengths += str(duration) + " " + path2str(r.path);
                sounds_lengths_update = true;
            }
            else duration = it->second;

            int spaces = 0;
            for (char c: r.title)
            if (c == ' ') spaces++;

            if (duration > 20.0
            or  duration > 4.0 and spaces <= 1)
            report::errors.log += link(&r) + " " +
            red(str(duration) + " sec");
        }

        if (sounds_lengths_update)
            sys::write("../data/"
           "sounds_lengths.txt",
            sounds_lengths);
    }

    struct sensecontrol
    {
        // sensless vocalization fits for any sense,
        // if there is another one with provided sense
        // then it will be reported

        hashmap<str, array<ent>> course;
        hashmap<str, array<res>> audios;
        hashmap<str, array<res>> videos;

        auto& map (Res& r) { return r.vocal() ? audios : videos; }

        sensecontrol
        (
            array<Ent>& entries,
            array<Res>& resources
        )
        {
            for (auto& entry: entries) if (entry.sense != "")
            for (str s: entry.matches) if (entry.sense != s)
                course[s] += &entry;

            for (auto& r: resources)
            for (auto& e: r.Entries())
            {
                str title = e;
                str sense = title.extract_from("@");
                if (sense != "") map(r)[title] += &r;
            }

            // add sensless also
            // for listing of occurrences

            for (auto& entry: entries) if (entry.sense == "" or entry.sense == "@")
            for (str s: entry.matches)
                if (course.contains(s)
                or  audios.contains(s)
                or  videos.contains(s))
                    course[s] += &entry;

            for (auto& r: resources)
            for (auto& e: r.Entries())
            {
                str title = e;
                str sense = title.extract_from("@");
                if (sense == "" and not r.vocal())
                if (course.contains(title)
                or  map(r).contains(title))
                    map(r)[title] += &r;
            }

            for (auto& entry: entries)
            {
                if (entry.sense == "" and
                not entry.eng.contains(str(u8" → ")))
                for (str s: entry.matches)
                if (audios.contains(s)
                or  videos.contains(s) and
                not entry.opt.external.contains("HEAD"))
                {
                    report::errors += bold(
                    red("sensless: ")) +
                    link(entry);

                    if (course.contains(s))
                    for (ent e: course[s]) if (e != &entry)
                    report::errors += link(e);

                    if (audios.contains(s))
                    for (res r: audios[s])
                    report::errors += link(r);

                    if (videos.contains(s))
                    for (res r: videos[s])
                    report::errors += link(r);
                }
            }

            hashset<str> reported;

            for (auto& r: resources)
            for (auto& e: r.Entries())
            {
                str title = e;
                str sense = title.extract_from("@");
                if (sense != "") continue;

                if (map(r).contains(title)
                or  course.contains(title) and not r.vocal())
                {
                    if ( // prevent multiple reports
                    reported.contains(title)) continue;
                    reported.emplace(title);

                    report::errors += bold(
                    red("sensless: ")) +
                    link(&r);

                    if (course.contains(title))
                    for (ent e: course[title])
                    report::errors += link(e);

                    if (map(r).contains(title))
                    for (res x: map(r)[title]) if (x != &r)
                    report::errors += link(x);
                }
            }
        }

        // do not report immediately
        // all unused senses of resources,
        // allow some senses for dictionary,
        // report only if resource wasn't used at all

        void report_unused (hashset<res>& unused_resources)
        {
            for (auto medio: {&audios, &videos})
            for (auto& [s, rr]: *medio) for (res r: rr)
            if  (unused_resources.contains(r))
            {
                unused_resources.erase(r);

                report::errors += bold(
                red("unused sense: ")) +
                link(r);

                if (course.contains(s))
                for (ent e: course[s])
                report::errors += link(e);

                if (audios.contains(s))
                for (res x: audios[s]) if (x != r)
                report::errors += link(x);

                if (videos.contains(s))
                for (res x: videos[s]) if (x != r)
                report::errors += link(x);
            }
        }
    };

    auto all_entries(res r)
    {
        auto entries = r->entries;
        auto title = doc::html::untagged(r->title);

        if ((r->kind == "audio"
        and entries.size() == 0
        and not r->options.contains("=")
        and not r->options.contains("=="))
        or  entries.contains("+"))
            entries += eng::parser::entries(
            app::vocabulary, title,
            r->options.contains
            ("Case"));
        else
        if (title.contains("/"))
        for (str s: title.split_by("/"))
            entries += s;

        entries.try_erase("+");
        return entries;
    }

    str cliplink (res r, hashset<str>& course_vocabulary)
    {
        array<str> missed;
        for (str x: all_entries(r))
        if  (x.size() >= 2 // skip "," "!" "?"
        and not x.contains(" ") // "see you"
        and not course_vocabulary.contains(
            eng::lowercased(simple(x))))
            missed += x;

        str s = r->full();
        if (r->options.contains("sound"))
            s = "[" + s + "]";

        str abstract = r->abstract;
        if (r->options.contains("sound"))
            abstract += " # SOUND";

        return linked(
        dark(html(s)) + red(" [" + str(missed, "] [") + "]"),
        "clipboard://: " + abstract +
        "file://" + str(r->path));
    }

    void suggestions
    (
        content::out::course& course,
        hashset<res>& unused_resources)
    {
        hashmap<str,
        array<res>> unused_resources_vocabulary;
        for (res r: unused_resources)
        {
            int spaces = 0;
            for (char c: r->abstract)
            if  (c == ' ') spaces++;

            if (spaces == 0 and r->vocal())
                continue;

            if (spaces == 1 and r->vocal())
            if (r->abstract.starts_with("a "  )
            or  r->abstract.starts_with("an " )
            or  r->abstract.starts_with("the ")
            or  r->abstract.starts_with("to " ))
                continue;

            if (spaces == 2 and r->vocal())
            if (r->abstract.starts_with("to be "))
                continue;

            auto entries = all_entries(r);

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

            for (str form: entry.vocabulary)
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
                    for (str x: all_entries(r))
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

    void order_check (content::out::course& course)
    {
        hashmap<str, array<ent>> phrase_vocabulary;
        hashmap<str, array<ent>> single_vocabulary;

        for (auto& entry: course.entries)
        {
            bool single = true;
            if  (not entry.opt.external.contains("HEAD")
            and  not entry.eng.contains(str(u8"→")))
            for (str phrase: entry.phrases)
            if (phrase.contains(" ")) {
                single = false;
                break; }

            if (single and not entry.opt.external.contains("HEAD"))
            {
                std::set<ent> ee;
                for (str form: entry.phrases)
                if (phrase_vocabulary.contains(form) and
                not single_vocabulary.contains(form))
                for (ent e: phrase_vocabulary[form])
                    ee.emplace(e);

                if (not ee.empty())
                {
                    report::orders += "";
                    for (ent e: ee)
                    report::orders += link(*e);
                    report::orders += link(entry);
                }
            }

            array<str> forms;
            for (str form: entry.vocabulary)
            if (form.size() >= 2)
            forms += form;
            forms.deduplicate();

            auto & v = single?
            single_vocabulary:
            phrase_vocabulary;
            for (str form: forms)
            v[form] += &entry;
        }
    }
}
