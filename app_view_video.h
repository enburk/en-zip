#pragma once
#include "app_view_text.h"
namespace app::video
{
    struct player:
    widget<player>
    {
        sfx::vudio::player video;
        sfx::media::medio medio;
        text::view title;

        gui::time stay;
        gui::time start;
        sys::thread thread;
        media::index video_index;
        array <byte> video_bytes;
        array<media::index> audio_index;
        array<array <byte>> audio_bytes;
        double slowdown = 1.0;
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

        player () { title.alignment = xy{pix::center, pix::top}; }

       ~player () { reset(); }

        void reset ()
        {
            try {
            thread.stop = true;
            thread.join();
            thread.check(); }
            catch (...) {}
            video.reset();
            medio.done();
        }

        void load ()
        {
            reset();
            medio.load();
            thread = [this](std::atomic<bool>& cancel)
            {
                video_bytes = media::bytes(video_index);
                audio_bytes = media::bytes(audio_index);
            };
        }

        void play ()
        {
            if (medio.play()) {
                video.play();
                logs::media << media::log(video_index);
                logs::media << media::log(audio_index);
                start = gui::time::now;
            }
        }
        void stop ()
        {
            if (medio.stop())
                video.stop();
        }

        void fit (xy maxsize, gui::time time={}) override
        {
            xy resolution {
            video_index.location.size_x,
            video_index.location.size_y};

            xy pixsize = resolution.fit(maxsize);
            xy txtsize = maxsize;
            if (pixsize.x > 0)
                txtsize.x =
                pixsize.x;

            title.resize(txtsize);
            txtsize = title.textsize();

            if (pixsize.y + txtsize.y > maxsize.y)
            {
                pixsize = maxsize;
                pixsize.y -= txtsize.y;
                pixsize = resolution.fit(pixsize);
            }

            int w = max(pixsize.x, txtsize.x);
            int h = pixsize.y + txtsize.y;
            int m = pixsize.y;

            video.coord = xywh(w/2 - pixsize.x/2, 0, pixsize.x, pixsize.y);
            title.coord = xywh(0, m, w, txtsize.y);
            resize(xy(w,h), time);
        }

        void on_change (void* what) override
        {
            if (what == & title)
                clicked = title.
                clicked,
                notify();

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
                if (title.link != "")
                start = gui::time::now;
            }

            if (what == &playing
            and video.status == state::finished
            and start + stay < gui::time::now)
            {
                medio.done();
                stay.ms = int(slowdown*
                stay.ms);
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