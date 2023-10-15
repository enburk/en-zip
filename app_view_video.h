#pragma once
#include "app_view_text.h"
namespace app::video
{
    struct player:
    widget<player>
    {
        sfx::vudio::player video;
        text::view title;

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

        void load (str html,
            array<byte> video_bytes,
            array<array<byte>> audio_bytes)
        {
            video.load(
            std::move(video_bytes),
            std::move(audio_bytes));
            title.html = html;
        }
        template<class... array_of_bytes>
        void load (str html,
            array<byte> video_bytes,
            array_of_bytes... audio_bytes)
        {
            video.load(
            std::move(video_bytes),
            std::move(audio_bytes)...);
            title.html = html;
        }

        void play () { video.play (); }
        void stop () { video.stop (); }
        void reset() { video.reset(); }

        void fit (xy resolution, xy maxsize)
        {
            xy pixsize = maxsize;
            pixsize = resolution.fit(pixsize);

            xy txtsize = maxsize;
            txtsize.y -= pixsize.y;

            title.coord = txtsize;
            txtsize = title.textsize();

            if (pixsize.y + txtsize.y > maxsize.y)
            {
                xy pixsize = maxsize;
                pixsize.y -= txtsize.y;
                pixsize = resolution.fit(pixsize);
            }

            int w = max(pixsize.x, txtsize.x);
            int h = pixsize.y + txtsize.y;

            video.coord = xywh(w/2 - pixsize.x/2, 0, pixsize.x, pixsize.y);
            title.move_to(xy(0, pixsize.y));
            resize(xy(w,h));
        }
    };
}