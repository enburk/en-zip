#pragma once
#include "app_view_text.h"
#include "app_view_video.h"
namespace app::two::video
{
    struct player:
    widget<player>
    {
        app::video::player video;
        text::view& script = video.title;
        gui::text::cell credia;
        gui::text::cell credib;
        gui::text::cell credic;
        gui::text::cell credid;
        text::cell credit;
        int clicked = 0;

#define using(x) decltype(video.x)& x = video.x;
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

        void play () { video.play (); }
        void stop () { video.stop (); }
        void reset() { video.reset(); }

        bool playable () { return video.medio.playable(); }

        void load (
            media::index video_index,
            media::index audio_index)
        {
            //video.mute = true;
            video.stay = gui::time{3000 + aux::random(0, 1000)};
            video.audio_index = audio_index;
            video.video_index = video_index;
            video.load();

            str c = video_index.credit;
            str s = video_index.options.contains("texted") ? "" :
                    video_index.title;

            s = media::canonical(s);
            c = media::canonical(c);
            c = small(small(c));

            script.html = s;
            credia.html = white(c);
            credib.html = white(c);
            credic.html = white(c);
            credid.html = white(c);
            credit.html = black(c);

            credia.hide();
            credib.hide();
            credic.hide();
            credid.hide();
            credit.hide();
        }

        void fit (xy size, gui::time time={}) override
        {
            int w = size.x;
            int h = size.y;
            int l = gui::metrics::line::width;

            video.fit(xy{w,h});
            w = video.coord.now.w;
            h = video.coord.now.h;

            credit.alignment = xy{pix::left, pix::top};
            credit.resize(xy{w,h});
            credit.resize(credit.text_size() + xy(10,0));

            video .move_to(xy{0,0});
            credit.move_to(video.video.coord.now.size - 
            credit.coord.now.size - xy{0,l});
            credia.coord = credit.coord.now + xy(-l,-l);
            credib.coord = credit.coord.now + xy(-l,+l);
            credic.coord = credit.coord.now + xy(+l,-l);
            credid.coord = credit.coord.now + xy(+l,+l);

            resize(xy{w,h}, time);
        }

        void on_change (void* what) override
        {
            if (what == &credit) clicked = credit.clicked, notify();
            if (what == &script) clicked = script.clicked, notify();
            if (what == &video ) clicked = video .clicked, notify();

            if (what == &skin)
                script.color = gui::skins[skin].dark.first;
 
            if (what == &volume)
                video.volume =
                volume;

            if (what == &mute)
                video.mute =
                mute;
        }

        bool mouse_sensible (xy) override { return true; }
        void on_mouse_hover_child (xy p) override { on_mouse_hover(p); }
        void on_mouse_hover (xy) override
        {
            credia.show();
            credib.show();
            credic.show();
            credid.show();
            credit.show();
        }
        void on_mouse_leave () override
        {
            credia.hide();
            credib.hide();
            credic.hide();
            credid.hide();
            credit.hide();
        }
    };
}