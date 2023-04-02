#pragma once
#include "app.h"
#include "app_dic_html.h"
namespace app::one
{
    struct entry:
    widget<entry>
    {
        sfx::media::medio   medio;
        sfx::media::player  vudio;
        app::dic::html_view script;
        app::dic::html_view credit;
        app::dic::html_view Script;
        app::dic::html_view Credit;

        property<int>  number = -1;
        property<bool> translated = false;

        gui::time stay;
        gui::time start;
        sys::thread thread;
        media::index video_index;
        media::index audio_index;
        array<byte> video_bytes;
        array<byte> audio_bytes;
        bool pixed = false;
        bool vocal = false;
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
            vudio.reset();
            medio.done();
        }

        void init ()
        {
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

            video_index = media::index{}; int vv = videos.size();
            audio_index = media::index{}; int aa = audios.size();

            if (vv>0) video_index = videos[aux::random(0, vv-1)];
            if (aa>0) audio_index = audios[aux::random(0, aa-1)];

            pixed = video_index != media::index{};
            vocal = audio_index != media::index{};
        }

        void translate ()
        {
            auto const& entry =
            app::one::course.entries[number];

            str html = entry.html(translated);
            str text = doc::html::untagged(html);

            start = gui::time::now;
            stay  = gui::time{(int)((1000 +
            video_index.title.size() * 30 +
            audio_index.title.size() * 00 +
            video_index.credit.size() * 10 +
            audio_index.credit.size() * 10 +
            video_index.comment.size() * 0 +
            audio_index.comment.size() * 0 +
            text.size() * 40) / speed)};

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
                if (not S.contains(s))
                script.html = c;
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

                credit.html = gray(small(c));
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

                Credit.html = gray(small(c));
            }
        }

        void load ()
        {
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

            video_index = media::index{}; int vv = videos.size();
            audio_index = media::index{}; int aa = audios.size();

            if (vv>0) video_index = videos[aux::random(0, vv-1)];
            if (aa>0) audio_index = audios[aux::random(0, aa-1)];

            pixed = video_index != media::index{};
            vocal = audio_index != media::index{};

            reset();
            medio.load();
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
                start = gui::time::now;
            }
        }
        void stop ()
        {
            if (medio.stop())
                vudio.stop();
        }

        int resize (int width)
        {
            int l = gui::metrics::line::width;
            int d = gui::metrics::text::height*7/10;

            xy size = pixed ? xy{
            video_index.location.size_x,
            video_index.location.size_y}:
            xy{};

            if (width < size.x) size = xy{
                width, width *
                size.y/size.x};

            script.alignment = xy{pix::left,   pix::top};
            credit.alignment = xy{pix::right,  pix::top};
            Script.alignment = xy{pix::center, pix::top};
            Credit.alignment = xy{pix::center, pix::top};

            script.coord = xywh(0, 0, size.x, max<int>());
            credit.coord = xywh(0, 0, size.x, max<int>());
            Script.coord = xywh(0, 0, size.x, max<int>());
            Credit.coord = xywh(0, 0, size.x, max<int>());

            int w1 = script.model.now->block.size.x;
            int w2 = credit.model.now->block.size.x;
            int h1 = script.model.now->block.size.y;
            int h2 = credit.model.now->block.size.y;
            int h3 = Script.model.now->block.size.y;
            int h4 = Credit.model.now->block.size.y;

            int y1 = size.y;
            int hh = h1 + h2;
            int y2 = h1;

            if (w1 + w2 + 2*d < size.x) {
                hh = max(h1, h2);
                y2 = 0; }

            vudio .move_to(xy{(width - size.x)/2, 0});
            credit.move_to(xy{0, y1 + y2});
            Script.move_to(xy{0, y1 + y2 + hh});
            Credit.move_to(xy{0, y1 + y2 + hh + h3});

            return y1 + y2 + hh + h3 + h4;
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size) resize(
                coord.now.w);

            if (what == &number)
            {
                init();
                translate();
                load();
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

            if (what == &playing)
            {
                if (script.link != ""
                or  credit.link != "")
                start = gui::time::now;
            }

            if (what == &playing
            and vudio.status == state::finished
            and start + stay < gui::time::now)
            {
                medio.done();
            }

            if (what == &playing
            or  what == &loading)
            {
                if (vudio.status == state::failed)
                medio.fail(vudio.error);
            }
 
            if (what == &volume)
                vudio.volume =
                volume;

            if (what == &mute)
                vudio.mute =
                mute;
        }
    };
}

