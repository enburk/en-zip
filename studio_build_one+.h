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
}
