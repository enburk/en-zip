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
        using idx = media::media_index;

        void reload () {}

        void reset (
            array<idx> const& selected_video,
            array<str> const& links)
        {
            text.forbidden_links = links;

            video.reset(selected_video, links);

            int l = gui::metrics::line::width;

            XY size;
            for (auto& video : selected_video)
            size.x = max (size.x, video.location.size_x + 6*l);
            size.y = video.height(size.x);

            refresh_video(size);
        }

        void refresh_video (XY size)
        {
            int maxwidth = coord.now.size.x * 2/3;
            if (maxwidth < size.x) size = XY (
                maxwidth, video.height(maxwidth));

            text.margin_right = size;
            int d = text.scroll.y.alpha.to == 0 ?
                0 : text.scroll.y.coord.now.w;

            video.coord = XYWH(
                coord.now.size.x - size.x - d, 0,
                size.x, size.y
            );
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                text.coord = coord.now.local();
                refresh_video(video.coord.now.size);
            }
            if (what == &skin)
            {
                text.view.canvas.color = gui::skins[skin].ultralight.first;
            }
            if (what == &video)
            {
                text.margin_right = video.coord.now.size;
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &text ) { clicked = text .clicked; notify(); }
            if (what == &video) { clicked = video.clicked; notify(); }
        }
    };
}
