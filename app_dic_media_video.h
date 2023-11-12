#pragma once
#include "eng_parser.h"
#include "app_view_text.h"
#include "app_view_video.h"
#include "app_dic_media.h"
namespace app::dic::video
{
    struct player:
    widget<player>
    {
        gui::canvas canvas;
        gui::frame  frame1;
        gui::frame  frame2;
        app::video::player video;
        text::view& script = video.title;
        text::view  credit;
        gui::button Play;
        gui::button Stop;
        gui::button prev;
        gui::button next;

        media::index index;
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

        void load (
            media::index video_index,
            media::index audio_index,
            array<str> links)
        {
            // if that same image
            // is used for another word then do next:
            // actualize emboldened links,
            // and return

            bool same =
            index == video_index;
            index =  video_index;

            script.forbidden_links = links;
            credit.forbidden_links = links;

            str s = index.title;
            str c = index.credit;

            s = eng::parser::embolden(s, links);

            c = media::canonical(c);
            s = media::canonical(s);

            if (index.comment != "")
                s += "<br>" + dark(
                media::canonical(
                index.comment));

            script.html = s;
            credit.html = gray(small(c));

            video.stay = gui::time{3000 +
            index.title.size() * 40 +
            index.credit.size() * 10 +
            index.comment.size() * 20};

            if (same) return;

            video.mute = true;
            video.audio_index = audio_index;
            video.video_index = video_index;
            video.load();

            Play.hide();
            Stop.hide();
        }

        void fit (xy size, gui::time t={}) override
        {
            int l = gui::metrics::line::width;
            int W = gui::metrics::text::height*20;
            int d = gui::metrics::text::height*7/10;
            int w = size.x; w -= 6*l;
            int h = size.y; h -= 6*l;
            h -= d; // for credit

            script.alignment = xy{pix::left, pix::top};
            credit.alignment = xy{pix::left, pix::top};

            int maxwidth = index.location.size_x;
            if (maxwidth < W*80/100)
                maxwidth = W;

            w = min(w, maxwidth);
            video.fit(xy{w,h});
            w = video.coord.now.w;
            h = video.coord.now.h;

            credit.resize(xy{w,h});

            int w1 = script.textsize().x;
            int w2 = credit.textsize().x;

            w += 6*l;
            h += 6*l;
            if (w1 + 7*d + w2 > w)
            h += d;

            Play.resize(xy{d*3/2, d-d/7});
            Stop.resize(xy{d*3/2, d-d/7});
            prev.resize(xy{d*3/2, d-d/7});
            next.resize(xy{d*3/2, d-d/7});
            prev.text.shift = xy{0, -d/3};
            next.text.shift = xy{0, -d/3};

            xyxy r (0, 0, w, h);
            frame1.coord = r; r.deflate(frame1.thickness.now);
            frame2.coord = r; r.deflate(frame2.thickness.now);
            canvas.coord = r; r.deflate(frame2.thickness.now);

            video .move_to(r.lt);
            credit.move_to(r.rb - xy{d*10/2, d} - xy{w2,0});
            Play  .move_to(r.rb - xy{d* 9/2, d} + xy{0, d/7});
            Stop  .move_to(r.rb - xy{d* 9/2, d} + xy{0, d/7});
            prev  .move_to(r.rb - xy{d* 6/2, d} + xy{0, d/7});
            next  .move_to(r.rb - xy{d* 3/2, d} + xy{0, d/7});

            resize(xy{w,h});
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                rgba c = rgba::white;
                int r = gui::metrics::text::height/16;//8;
                Play.icon.load(pix::util::icon("play.play",  Play.coord.now.size, c, r));
                Stop.icon.load(pix::util::icon("play.pause", Stop.coord.now.size, c, r));
            }
            if (what == &skin)
            {
                frame1.color = gui::skins[skin].ultralight.first;
                frame2.color = gui::skins[skin].normal.first;
                canvas.color = gui::skins[skin].light.first;

                prev.text.html = monospace(bold(u8"←"));
                next.text.html = monospace(bold(u8"→"));
            }

            if (what == &credit) { clicked = credit.clicked; notify(); }
            if (what == &script) { clicked = script.clicked; notify(); }

            if (what == &next) { clicked = -1; notify(); }
            if (what == &prev) { clicked = -2; notify(); }
 
            if (what == &Play) { mute = false; play(); }
            if (what == &Stop) { mute = true;  stop(); }

            if (what == &video)
            {
                using sfx::media::state;
                if (video.duration.ms > 0)
                Stop.show(video.status == state::playing),
                Play.show(video.status == state::finished
                       or video.status == state::paused
                       or video.status == state::ready);
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