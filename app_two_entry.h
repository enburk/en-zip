#pragma once
#include "app.h"
#include "app_view_text.h"
#include "app_view_video.h"
namespace app::two
{
    struct entry:
    widget<entry>
    {
        gui::frame frame;
        video::player player;
        text::view script;
        text::view credia;
        text::view credib;
        text::view credic;
        text::view credid;
        text::view credit;

        property<int>  number = -1;
        property<bool> translated = false;

        media::index audio_index;
        media::index sound_index;
        media::index video_index;
        bool new_topic = false;
        bool new_chain = false;
        bool pixed = false;
        bool vocal = false;
        bool sound = false;
        int clicked = 0;

#define using(x) decltype(player.x)& x = player.x;
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

        void load () { player.load (); }
        void play () { player.play (); }
        void stop () { player.stop (); }
        void reset() { player.reset(); }

        void init ()
        {
            reset();

            array<media::index> audios;
            array<media::index> sounds;
            array<media::index> videos;

            auto range = mediadata.entries_two. // !!!
            equal_range(media::entry_index{number, 0},
                [](auto a, auto b){ return
                a.entry < b.entry; });

            for (auto [entry, media]: range)
            {
                auto& i =
                mediadata.
                media_index[media];
                if (i.kind == "video") videos += i;
                if (i.kind == "audio")
                {
                    if (i.options.
                    contains("sound"))
                    sounds += i; else
                    audios += i;
                }
            }

            audio_index = {}; int aa = audios.size();
            sound_index = {}; int ss = sounds.size();
            video_index = {}; int vv = videos.size();

            if (aa>0) audio_index = audios[aux::random(0, aa-1)];
            if (ss>0) sound_index = sounds[aux::random(0, ss-1)];
            if (vv>0) video_index = videos[aux::random(0, vv-1)];

            for (auto& audio: audios) logs::audio << log(audio);
            for (auto& sound: sounds) logs::audio << log(sound);
            for (auto& video: videos) logs::video << log(video);

            auto const& entry =
            course.entries[number];
            if (entry.opt.external.
                contains("SOUND"))
                audio_index =
                media::index{},
                aa = 0;

            pixed = vv > 0;
            vocal = aa > 0;
            sound = ss > 0;

            player.audio_index.clear();
            player.audio_index += audio_index;
            player.audio_index += sound_index;
            player.video_index  = video_index;

            credia.hide();
            credib.hide();
            credic.hide();
            credid.hide();
            credit.hide();
            frame.hide();
        }

        void translate ()
        {
            if (number == -1)
                return;

            auto const& entry =
            course.entries[number];

            str html = entry.html(translated);

            if (vocal
            and entry.rus == ""
            and entry.eng.size() < audio_index.title.size())
            {
                html = media::canonical(audio_index.title);
                if (audio_index.credit != "") html += "<br>"
                "<div style=\"line-height: 20%\"><br></div>" +
                gray(small(audio_index.credit));
            }

            str text = doc::html::untagged(html);

            player.stay = gui::time{1000 +
            video_index.title.size() * 10 +
                text.size() * 10};

            script.html = html;
            credit.html = "";

            if (pixed)
            {
                str t = video_index.title;
                str c = media::canonical(t);
                str s = eng::lowercased(c);
                str S = eng::lowercased(text);
                str E = eng::lowercased(entry.eng);
                E.replace_all("\\\\", "/");
                E.replace_all("'''", "");
                E.replace_all("''", "");
                E.replace_all("~", "");
                E.replace_all("[", "");
                E.replace_all("]", "");
                E.canonicalize();
                if (not S.contains(s)
                and not E.contains(s))
                player.title.html =
                    small(c);
            }
            if (pixed)
            {
                str
                c = video_index.credit;
                c = media::canonical(c);
                c = small(small(c));
                credia.html = white(c);
                credib.html = white(c);
                credic.html = white(c);
                credid.html = white(c);
                credit.html = black(c);
            }
        }

        int resize (int w, int h)
        {
            int l = gui::metrics::line::width;
            int d = gui::metrics::text::height;

            if (w < l+l) return 0; w -= l+l; 
            if (h < l+l) return 0; h -= l+l; 

            script.alignment = xy{pix::center, pix::top};
            credit.alignment = xy{pix::left,   pix::top};

            script.coord = xywh(l, l, w, h);
            credit.coord = xywh(l, l, w, h);

            script.resize(script.textsize());
            credit.resize(credit.textsize() + xy(10,0));

            player.fit(xy{w, h - script.coord.now.h});

            xy psize = player.coord.now.size;
            xy ssize = script.coord.now.size;
            xy csize = credit.coord.now.size;

            player.move_to(xy(l + w/2 - psize.x/2, l));
            script.move_to(xy(l + w/2 - ssize.x/2, l + psize.y));
            credit.move_to(xy(l + w   - csize.x  , l + 
            player.title.coord.now.y  - csize.y));

            credia.coord = credit.coord.now + xy(-l,-l);
            credib.coord = credit.coord.now + xy(-l,+l);
            credic.coord = credit.coord.now + xy(+l,-l);
            credid.coord = credit.coord.now + xy(+l,+l);

            return l + psize.y + ssize.y + l;
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
            if (what == &player) { clicked = player.clicked; notify(); }
        }

        bool mouse_sensible (xy) override { return true; }
        void on_mouse_hover_child (xy p) override { on_mouse_hover(p); }
        void on_mouse_hover (xy) override
        {
            frame .show();
            credia.show();
            credib.show();
            credic.show();
            credid.show();
            credit.show();
        }
        void on_mouse_leave () override
        {
            frame .hide();
            credia.hide();
            credib.hide();
            credic.hide();
            credid.hide();
            credit.hide();
        }
        void on_mouse_click (xy, str button, bool down) override
        {
            if (button == "left" and down)
            player.stop(),
            player.play(),
            clicked = -1,
            notify();
        }
    };
}

