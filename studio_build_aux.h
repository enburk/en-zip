#pragma once
#include "app.h"
#include "content_entry+.h"
namespace studio
{
    using Res = media::resource;
    using res = media::resource*;
    using Ent = content::out::entry;
    using ent = content::out::entry*;

    struct voc
    {
        array<ent> entries;
        array<res> resources;
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

    struct sensecontrol
    {
        // sensless vocalization fits for any sense,
        // if there is another one with provided sense
        // then it will be reported

        hashmap<str, array<ent>> course;
        hashmap<str, array<res>> audios;
        hashmap<str, array<res>> videos;

        auto& map (Res& r) { return r.vocal() ? audios : videos; }

        array<str>& errors;

        sensecontrol
        (
            array<Ent>& entries,
            array<Res>& resources,
            array<str>& errors
        )
        : errors(errors)
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
                    errors += bold(
                    red("sensless: ")) +
                    link(entry);

                    if (course.contains(s))
                    for (ent e: course[s]) if (e != &entry)
                    errors += link(e);

                    if (audios.contains(s))
                    for (res r: audios[s])
                    errors += link(r);

                    if (videos.contains(s))
                    for (res r: videos[s])
                    errors += link(r);
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

                    errors += bold(
                    red("sensless: ")) +
                    link(&r);

                    if (course.contains(title))
                    for (ent e: course[title])
                    errors += link(e);

                    if (map(r).contains(title))
                    for (res x: map(r)[title]) if (x != &r)
                    errors += link(x);
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

                errors += bold(
                red("unused sense: ")) +
                link(r);

                if (course.contains(s))
                for (ent e: course[s])
                errors += link(e);

                if (audios.contains(s))
                for (res x: audios[s]) if (x != r)
                errors += link(x);

                if (videos.contains(s))
                for (res x: videos[s]) if (x != r)
                errors += link(x);
            }
        }
    };

    generator<str> wrong_words(content::out::entry const& entry)
    {
        if  (not entry.opt.internal.contains("sic!"))
        for (str sssss: entry.en*entry.us*entry.uk)
        for (str ssss: sssss.split_by("/"))
        for (str sss: ssss.split_by("|"))
        for (str ss: sss.split_by(" "))
        for (str s: ss.split_by("\\"))
        {
            s.erase_all('(');
            s.erase_all(',');
            s.erase_all('.');
            s.erase_all(':');
            s.erase_all(';');
            s.erase_all('?');
            s.erase_all('!');
            s.erase_all('"');
            s.erase_all(')');
            s.replace_all(u8"‘", "");
            s.replace_all(u8"’", "");
            if (s.ends_with("s'")) s.truncate();
            if (s.ends_with("'s")) s.truncate(), s.truncate();
            str w = eng::lowercased(s);
            str ww = eng::lowercased(ss);
            str www = eng::lowercased(sss);
            if (not app::vocabulary.contains(w)
            and not app::vocabulary.contains(s)
            and not app::vocabulary.contains(ww)
            and not app::vocabulary.contains(ss)
            and not app::vocabulary.contains(www)
            and not app::vocabulary.contains(sss))
            co_yield w;
        }
    }
}