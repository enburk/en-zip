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

    using res = media::resource*;
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

    str link (res r)
    {
        return linked(
        dark(html(r->full())),
        "file://" + str(r->path));
    }
    str link (ent entry)
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

    void suggestions
    (
        content::out::course course,
        std::unordered_set<res> unused_resources,
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
