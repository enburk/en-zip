﻿#pragma once
#include "app.h"
#include <unordered_set>
namespace studio::one
{
    namespace report
    {
        array<str> errors;
        array<str> anomal;
        array<str> audiom, audiop, audioq;
        array<str> videom, videop, videoq;
        void load ()
        {
            std::filesystem::path dir = "../data/report";
            errors = sys::in::optional_text(dir/"one_errors.dat").lines();
            anomal = sys::in::optional_text(dir/"one_anomal.dat").lines();
            audiom = sys::in::optional_text(dir/"one_audiom.dat").lines();
            videom = sys::in::optional_text(dir/"one_videom.dat").lines();
            audiop = sys::in::optional_text(dir/"one_audiop.dat").lines();
            videop = sys::in::optional_text(dir/"one_videop.dat").lines();
            audioq = sys::in::optional_text(dir/"one_audioq.dat").lines();
            videoq = sys::in::optional_text(dir/"one_videoq.dat").lines();
        }
        void save ()
        {
            std::filesystem::path dir = "../data/report";
            sys::out::write(dir/"one_errors.dat", errors);
            sys::out::write(dir/"one_anomal.dat", anomal);
            sys::out::write(dir/"one_audiom.dat", audiom);
            sys::out::write(dir/"one_videom.dat", videom);
            sys::out::write(dir/"one_audiop.dat", audiop);
            sys::out::write(dir/"one_videop.dat", videop);
            sys::out::write(dir/"one_audioq.dat", audioq);
            sys::out::write(dir/"one_videoq.dat", videoq);
        }
        void clear()
        {
            errors.clear();
            anomal.clear();
            audiom.clear();
            videom.clear();
            audiop.clear();
            videop.clear();
            audioq.clear();
            videoq.clear();
        }
    }

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
        if (true) sys::out::file("../data/course.dat") << course.root;
        if (true) sys::out::file("../data/course_entries.dat") << course.entries;

        report::clear();
        report::errors = course.errors;
        report::anomal = course.anomal;
        
        if (not report::errors.empty()) {
        err << red(bold("ONE ERRORS:"));
        err << report::errors; }

        using res = media::resource*;

        out << dark(bold("ONE: SCAN ENTRIES..."));

        std::unordered_map<str, array<res>> vocab;
        for (auto& entry: course.entries)
        for (str s: entry.vocabulary)
            vocab.emplace(s,
            array<res>{});

        out << dark(bold("ONE: SCAN RESOURCES..."));

        std::unordered_set<res> unused_resources;
        for (auto& r: data.resources)
        {
            array<str> sss;
            sss += r.abstract; if (r.kind == "video")
            sss += r.entries;
            for (str ss: sss)
            for (str s: ss.split_by("/"))
            {
                auto it = vocab.find(s);
                if (it != vocab.end())
                    it->second += &r;
                else
                if (r.weight < 20
                or  r.kind == "video")
                unused_resources.
                    emplace(&r);
            }
        }

        out << dark(bold("ONE: CHECK FULFILMENT..."));

        for (auto [i, entry]: enumerate(course.entries))
        {
            auto en = entry.en;
            auto uk = entry.uk;
            auto us = entry.us;
            bool vi = entry.opt.internal.contains("pix-");
            for (str& s: entry.vocabulary)
            for (res& r: vocab[s])
            {
                data.one_add(i, r);
                if (r->kind == "audio") {
                if (r->options.contains("uk")) uk.try_erase(r->abstract);
                if (r->options.contains("us")) us.try_erase(r->abstract);
                /******** in any case *******/ en.try_erase(r->abstract); }
                if (r->kind == "video") vi = true;
            }
            str ens = red(bold(" en: " + html(str(en, ", "))));
            str uks = red(bold(" uk: " + html(str(uk, ", "))));
            str uss = red(bold(" us: " + html(str(us, ", "))));
            if (not en.empty()) report::audiom += linked(html(entry.eng) + ens, entry.link);
            if (not uk.empty()) report::audiom += linked(html(entry.eng) + uks, entry.link);
            if (not us.empty()) report::audiom += linked(html(entry.eng) + uss, entry.link);
            if (not vi        ) report::videom += linked(html(entry.eng), entry.link);
        }

        out << dark(bold("ONE: MAKE SUGGESTIONS..."));

        std::unordered_map<str,
        array<res>> unused_resources_vocab;
        for (res r: unused_resources)
        {
            auto entries = r-> entries;

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
            unused_resources_vocab[s] += r;
        }

        std::unordered_set<str> current_vocab;

        for (auto& entry: course.entries)
        {
            array<res> audios;
            array<res> videos;

            for (str& s: entry.vocabulary)
            {
                current_vocab.emplace(s);
                auto it = unused_resources_vocab.find(s);
                if (it == unused_resources_vocab.end())
                    continue;

                for (res r: it->second)
                {
                    if (not unused_resources.contains(r))
                        continue;

                    bool well_known = true;
                    for (str x: r->entries)
                    if  (not current_vocab.contains(x))
                        well_known = false;

                    if (well_known) {
                    if (r->kind == "audio") audios += r;
                    if (r->kind == "video") videos += r;
                    unused_resources.erase(r); }
                }
            }

            if (not audios.empty())
            {
                report::audiop +=
                linked(html(entry.eng) + " " +
                light(small(entry.link)), entry.link);

                for (res r: audios)
                report::audiop += 
                dark(html(r->title));
            }
            if (not videos.empty())
            {
                report::videop +=
                linked(html(entry.eng) + " " +
                light(small(entry.link)), entry.link);

                for (res r: videos)
                report::videop +=
                dark(html(r->title));
            }
        }

        std::multimap<int, res>
            weighted_unused_resources;
        for (res r:  unused_resources)
            weighted_unused_resources.
            emplace(r->weight, r);

        for (auto[weight, r]: weighted_unused_resources)
        {
            if (r->kind == "audio") report::audioq += r->title;
            if (r->kind == "video") report::videoq += r->title;
        }

        out << dark(bold("ONE: PREVENT REPEATING..."));

        // list of single resources

        for (auto& entry: course.entries)
        {
            // if entry has sigle resource
        }

        for (auto& entry: course.entries)
        {
            // if entry has multiple resources
        }

        report::save();
    }
}
