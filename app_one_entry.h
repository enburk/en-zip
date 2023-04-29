#pragma once
#include "app.h"
#include "app_dic_html.h"
namespace app::one
{
    struct entry:
    widget<entry>
    {
        gui::frame frame;
        sfx::media::medio  medio;
        sfx::media::player vudio;
        dic::html_view script;
        dic::html_view credit;
        dic::html_view Script;
        dic::html_view Credit;

        property<int>  number = -1;
        property<bool> translated = false;

        gui::time Stay;
        gui::time stay;
        gui::time start;
        sys::thread thread;
        media::index video_index;
        media::index audio_index;
        array<byte> video_bytes;
        array<byte> audio_bytes;
        bool new_topic = false;
        bool new_chain = false;
        bool pixed = false;
        bool vocal = false;
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
            vudio.reset();
            medio.done();
        }

        void init ()
        {
            reset();

            array<media::index> audios;
            array<media::index> videos;

            auto range = mediadata.entries_one.
            equal_range(media::entry_index{number, 0},
                [](auto a, auto b){ return
                a.entry < b.entry; });

            for (auto [entry, media]: range) {
                auto& i = mediadata.media_index[media];
                if (i.kind == "audio") audios += i;
                if (i.kind == "video") videos += i; }

            for (auto& audio: audios) logs::audio << log(audio);
            for (auto& video: videos) logs::video << log(video);

            video_index = media::index{}; int vv = videos.size();
            audio_index = media::index{}; int aa = audios.size();

            if (vv>0) video_index = videos[aux::random(0, vv-1)];
            if (aa>0) audio_index = audios[aux::random(0, aa-1)];

            pixed = vv > 0;
            vocal = aa > 0;

            frame.hide();
        }

        void translate ()
        {
            if (number == -1) return;

            auto const& entry =
            course.entries[number];

            str html = entry.html(translated);
            str text = doc::html::untagged(html);

            start = gui::time::now;
            stay  = gui::time{1000 +
            video_index.title.size() * 30 +
            audio_index.title.size() * 00 +
            video_index.credit.size() * 10 +
            audio_index.credit.size() * 10 +
            video_index.comment.size() * 0 +
            audio_index.comment.size() * 0 +
            text.size() * 40};

            script.html = "";
            Script.html = html;
            credit.html = "";
            Credit.html = "";

            if (pixed)
            {
                str t = video_index.title;
                str c = media::canonical(t);
                str s = eng::lowercased(c);
                str S = eng::lowercased(text);
                str E = eng::lowercased(entry.eng);
                E.replace_all("\\\\", "/");
                E.replace_all("[", "");
                E.replace_all("]", "");
                if (not S.contains(s)
                and not E.contains(s))
                script.html = small(c);
            }
            if (pixed)
            {
                str c = video_index.credit;
                c = media::canonical(c);

                str date;
                for (str option: video_index.options)
                if (option.starts_with("date "))
                    c += ", " + italic(
                    option.from(5));

                credit.html = gray(small(small(c)));
            }
            if (vocal)
            {
                str c = audio_index.credit;
                c = media::canonical(c);

                str date;
                for (str option: audio_index.options)
                if (option.starts_with("date "))
                    c += ", " + italic(
                    option.from(5));

                c.replace_all(", read by", "<br>read by");
                c.replace_all(", narrated by", "<br>narrated by");

                Credit.html = gray(small(small(c)));
            }
        }

        void load ()
        {
            medio.load();
            if (pixed or vocal)
            thread = [this](std::atomic<bool>& cancel)
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
                vudio.play();
                if (pixed) logs::media << media::log(video_index);
                if (vocal) logs::media << media::log(audio_index);
                stay  = gui::time{(int)(Stay.ms/speed)};
                start = gui::time::now;
            }
        }
        void stop ()
        {
            if (medio.stop())
                vudio.stop();
        }

        int resize (int w, int h)
        {
            int l = gui::metrics::line::width;
            int d = gui::metrics::text::height;

            if (w < l+l) return 0; w -= l+l; 
            if (h < l+l) return 0; h -= l+l; 

            script.scroll.y.mode = gui::scroll::mode::none;
            credit.scroll.y.mode = gui::scroll::mode::none;
            Script.scroll.y.mode = gui::scroll::mode::none;
            Credit.scroll.y.mode = gui::scroll::mode::none;

            script.alignment = xy{pix::left,   pix::top};
            credit.alignment = xy{pix::left,   pix::top};
            Script.alignment = xy{pix::center, pix::top};
            Credit.alignment = xy{pix::center, pix::top};

            credit.coord = xywh(l, l, w, max<int>());
            script.coord = xywh(l, l, w, max<int>());
            Script.coord = xywh(l, l, w, max<int>());
            Credit.coord = xywh(l, l, w, max<int>());

            int w1 = credit.model.now->block.size.x;
            int w2 = script.model.now->block.size.x;
            int h1 = credit.model.now->block.size.y;
            int h2 = script.model.now->block.size.y;
            int h3 = Script.model.now->block.size.y;
            int h4 = Credit.model.now->block.size.y;

            credit.alignment = xy{pix::right, pix::top};

            int y2 = h1;
            int hh = h1 + h2;

            if (w1 + w2 + d < w) {
                hh = max(h1, h2);
                y2 = 0; }

            h -= hh + h3 + h4;

            xy size = pixed ? xy{
            video_index.location.size_x,
            video_index.location.size_y}:
            xy{};

            if (w < size.x) size = xy{w, w*size.y/size.x};
            if (h < size.y) size = xy{h*size.x/size.y, h};

            vudio.coord = xywh(
            w/2-size.x/2 + l, l,
            size.x, size.y);

            credit.coord = xywh(l, l + size.y,           w, h1);
            script.coord = xywh(l, l + size.y + y2,      w, h2);
            Script.coord = xywh(l, l + size.y + hh,      w, h3);
            Credit.coord = xywh(l, l + size.y + hh + h3, w, h4);

            return l + size.y + hh + h3 + h4 + l;
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                resize(
                coord.now.w,
                coord.now.h);
                frame.coord =
                coord.now.local();
            }

            if (what == &skin)
                frame.color = 
                gui::skins[skin].
                focused.first;

            if (what == &number)
            {
                init();
                translate();
            }
            if (what == &translated)
            {
                translate();
            }

            if (what == &credit) { clicked = credit.clicked; notify(); }
            if (what == &script) { clicked = script.clicked; notify(); }
            if (what == &Credit) { clicked = Credit.clicked; notify(); }
            if (what == &Script) { clicked = Script.clicked; notify(); }

            using sfx::media::state;

            if (what == &loading
            and vudio.status == state::finished
            and thread.done)
            {
                try
                {
                    thread.join();
                    thread.check();

                    vudio.load(
                    std::move(video_bytes),    
                    std::move(audio_bytes));    
                }
                catch (std::exception const& e) {
                medio.fail(e.what()); }
            }

            if (what == &loading
            and vudio.status == state::ready
            and thread.done)
            {
                medio.stay();
            }

            if (what == &playing
            and vudio.status == state::finished
            and start + stay < gui::time::now)
            {
                medio.done();
            }

            if (what == &playing
            and vudio.status == state::playing
            and start + stay < gui::time::now)
            {
                int64_t e = vudio.elapsed.ms;
                int64_t d = vudio.duration.ms;
                int64_t s = (int)(200*speed);
                if (d-e < s)
                medio.done();
            }

            if (what == &playing
            or  what == &loading)
            {
                if (
                vudio.status == state::failed)
                medio.fail(vudio.error);
            }
 
            if (what == &volume)
                vudio.volume =
                volume;

            if (what == &mute)
                vudio.mute =
                mute;
        }

        bool mouse_sensible (xy) override { return true; }
        void on_mouse_hover (xy) override { } // frame.show(); }
        void on_mouse_leave (  ) override { frame.hide(); }
        void on_mouse_click (xy, str button, bool down) override
        {
            if (button == "left" and down)
                clicked = -1;
                notify();
        }
    };
}

