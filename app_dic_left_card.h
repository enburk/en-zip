#pragma once
#include "app_dic_media.h"
#include "app_dic_media_videos.h"
namespace app::dic::left
{
    struct card:
    widget<card>
    {
        html_view text;
        video::sequencer video;
        gui::property<bool> mute = false;
        using idx = media::media_index;
        xy video_max_size;
        int clicked = 0;

        void reload () {}

        void reset (str title,
            array<idx> const& selected_video,
            array<idx> const& selected_audio,
            array<str> const& links)
        {
            text.forbidden_links = links;

            video.reset(title, selected_video, selected_audio, links);

            int l = gui::metrics::line::width;

            for (auto& v: selected_video)
            video_max_size.x = max(
            video_max_size.x, v.location.size_x + 6*l);
            video_max_size.y = video.height(
            video_max_size.x);

            video_resize(video_max_size);
        }

        void video_resize (xy size)
        {
            int maxwidth = coord.now.size.x * 2/3;
            if (maxwidth < size.x) size = xy (
                maxwidth, video.height(
                maxwidth));

            video.resize(size);
            video_position();
        }

        void video_position ()
        {
            text.rwrap = array<xy>{
            xy{0, text.scroll.y.top},
            video.coord.now.size + xy{
            gui::metrics::line::width*3,
            gui::metrics::line::width*3}};

            int d = text.scroll.y.alpha.to == 0 ?
                0 : text.scroll.y.coord.now.w;

            video.move_to(xy(
            coord.now.size.x -
            video.coord.now.size.x -
            d, 0));
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                text.coord = coord.now.local();
                video_resize(video.coord.now.size);
            }
            if (what == &skin)
            {
                text.view.canvas.color = gui::skins[skin].ultralight.first;
            }
            if (what == &text.scroll.y
            or  what == &text.update_text)
            {
                video_position();
            }
            if (what == &text ) { clicked = text .clicked; notify(); }
            if (what == &video) { clicked = video.clicked; notify(); }
            if (what == &mute)
            {
                video.mute = mute.now;
            }
        }
    };
}

