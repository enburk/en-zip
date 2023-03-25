﻿#pragma once
#include "app.h"
#include "app_dic_html.h"
namespace app::one
{
    struct entry:
    widget<entry>
    {
        sfx::media::medio medio;
        sfx::media::player video;
        sfx::audio::player audio;
        app::dic::html_view script;
        app::dic::html_view credit;
        app::dic::html_view Script;
        app::dic::html_view Credit;

        gui::time stay;
        gui::time start;
        sys::thread thread;
        array<byte> video_bytes;
        array<byte> audio_bytes;
        bool pixed = false;
        str script_html;
        str credit_html;
        str Script_html;
        str Credit_html;
        int clicked = 0;

#define using(x) decltype(medio.x)& x = medio.x;
        using(mute)
        using(volume)
        using(loading)
        using(playing)
        using(resolution)
        using(duration)
        using(elapsed)
        using(status)
        using(error)
        #undef using

        ~entry () { reset(); }

        void reset ()
        {
            try {
            thread.stop = true;
            thread.join();
            thread.check(); }
            catch (...) {}
            video.reset();
            audio.reset();
            medio.done();
        }

        void load (int number)
        {
            array<media::index> audios;
            array<media::index> videos;

            auto range = mediadata.entries_one.equal_range(
            media::entry_index{number, 0}, [](auto a, auto b)
            { return a.entry < b.entry; });

            for (auto [entry, media]: range) {
            auto& index = mediadata.media_index[media];
            if (index.kind == "audio") audios += index;
            if (index.kind == "video") videos += index; }

            for (auto& x: videos) logs::video << media::log(x);
            for (auto& x: audios) logs::audio << media::log(x);

            media::index video_index; int vv = videos.size();
            media::index audio_index; int aa = audios.size();

            if (vv>0) video_index = videos[aux::random(0, vv-1)];
            if (aa>0) audio_index = audios[aux::random(0, aa-1)];

            auto const& entry = app::one::course.entries[number];

            pixed = video_index != media::index{};

            start = gui::time::now;
            stay  = gui::time{(int)((1000 +
            video_index.title.size() * 00 +
            audio_index.title.size() * 30 +
            video_index.credit.size() * 10 +
            audio_index.credit.size() * 10 +
            video_index.comment.size() * 0 +
            audio_index.comment.size() * 0)
                /speed)};




            str s = index.title;
            str c = index.credit;

            s = eng::parser::embolden(s, links);

            c = media::canonical(c);
            s = media::canonical(s);

            str date;
            for (str option: index.options)
            if (option.starts_with("date "))
                c += ", " + italic(
                option.from(5));

            if (index.comment != "")
                s += "<br>" + dark(
                media::canonical(
                index.comment));

            script.html = s;
            credit.html = gray(small(c));

            if (same) return;

            reset();
            medio.load();
            thread = [this,
            video_index, audio_index]
            (std::atomic<bool>& cancel)
            {
                auto source = [](int source){
                    return "../data/media/storage." +
                    std::to_string(source) + ".dat"; };

                if (video_index != media::index{})
                    video_bytes = sys::in::bytes(source(
                    video_index.location.source),
                    video_index.location.offset,
                    video_index.location.length);

                if (audio_index != media::index{})
                    audio_bytes = sys::in::bytes(source(
                    audio_index.location.source),
                    audio_index.location.offset,
                    audio_index.location.length);
            };
        }

        void play ()
        {
            if (medio.play()) {
                video.play();
                logs::media << media::log(index);
                start = gui::time::now;
                stay.ms = stay.ms
                *150/100;
            }
        }
        void stop ()
        {
            if (medio.stop())
                video.stop();
        }

        auto sizes (int width)
        {
            int l = gui::metrics::line::width;
            int d = gui::metrics::text::height*7/10;

            xy size {
            index.location.size_x,
            index.location.size_y};

            int maxwidth = width - 6*l;
            if (maxwidth < size.x) size = xy (
                maxwidth, maxwidth *
                size.y / size.x);

            credit.alignment = xy{pix::left, pix::top};

            script.coord = xywh(0, 0, size.x,     max<int>());
            credit.coord = xywh(0, 0, size.x-4*d, max<int>());

            int w1 = script.model.now->block.size.x;
            int w2 = credit.model.now->block.size.x;
            int h1 = script.model.now->block.size.y;
            int h2 = credit.model.now->block.size.y;

            credit.alignment = xy{pix::right, pix::center};

            int hh = h1 + h2;
            int y2 = h1;

            if (w1 + w2 + 2*d < size.x) {
                hh = max(h1, h2);
                y2 = 0; }

            hh = max(hh, d);

            int w = size.x + 6*l;
            int h = size.y + 6*l + hh;

            return std::tuple{w1, h1, w2, h2, size, w, h, d, y2};
        }

        int height (int width)
        {
            xywh script_save = script.coord.now;
            xywh credit_save = credit.coord.now;

            auto [w1, h1, w2, h2, size, w, h, d, y2] = sizes(width);

            script.coord = script_save;
            credit.coord = credit_save;

            return h;
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;

                auto [w1, h1, w2, h2, size, w, h, d, y2] = sizes(coord.now.size.x);

                xywh r (W-w, 0, w, h); // right-up
                frame1.coord = r; r.deflate(frame1.thickness.now);
                frame2.coord = r; r.deflate(frame2.thickness.now);
                canvas.coord = r; r.deflate(frame2.thickness.now);

                int r_x = r.x + r.w;
                int r_y = r.y + size.y;

                video .coord = xywh(r.x, r.y, size.x, size.y);
                script.coord = xywh(r.x, r_y, size.x, h1);
                credit.coord = xywh(r_x - d*7/2 - w2,  r_y + y2, w2, h2);
                prev  .coord = xywh(r_x - d*6/2, d/7 + r_y + y2, d*3/2, d - d/7);
                next  .coord = xywh(r_x - d*3/2, d/7 + r_y + y2, d*3/2, d - d/7);

                prev.text.shift = xy{0, -d/3};
                next.text.shift = xy{0, -d/3};

                script.scroll.x.mode = gui::scroll::mode::none;
                script.scroll.y.mode = gui::scroll::mode::none;
                credit.scroll.x.mode = gui::scroll::mode::none;
                credit.scroll.y.mode = gui::scroll::mode::none;
            }

            if (what == &skin)
            {
                frame1.color = gui::skins[skin].ultralight.first;
                frame2.color = gui::skins[skin].normal.first;
                canvas.color = gui::skins[skin].light.first;

                credit.alignment = xy{pix::right, pix::top};
                script.alignment = xy{pix::left,  pix::top};

                prev.text.html = monospace(bold(u8"←"));
                next.text.html = monospace(bold(u8"→"));
            }

            if (what == &credit) { clicked = credit.clicked; notify(); }
            if (what == &script) { clicked = script.clicked; notify(); }

            if (what == &next) { clicked = -1; notify(); }
            if (what == &prev) { clicked = -2; notify(); }

            using sfx::media::state;

            if (what == &loading
            and video.status == state::finished
            and thread.done)
            {
                try
                {
                    thread.join();
                    thread.check();

                    video.load(
                    std::move(video_bytes),    
                    std::move(audio_bytes));    
                }
                catch (std::exception const& e) {
                medio.fail(e.what()); }
            }

            if (what == &loading
            and video.status == state::ready
            and thread.done)
            {
                medio.stay();
            }

            if (what == &playing)
            {
                if (script.link != ""
                or  credit.link != "")
                start = gui::time::now;
            }

            if (what == &playing
            and video.status == state::finished
            and start + stay < gui::time::now)
            {
                medio.done();
            }

            if (what == &playing
            or  what == &loading)
            {
                if (video.status == state::failed)
                medio.fail(video.error);
            }
 
            if (what == &volume)
                video.volume =
                volume;

            if (what == &mute)
                video.mute =
                mute;
       }
    };
}

