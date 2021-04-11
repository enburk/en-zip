#pragma once
#include "app_dict_media.h"
#include "app_dict_card_card_videos.h"
namespace app::dict::card
{
    struct card : gui::widget<card>
    {
        html_view text;
        video::sequencer image;

        void reset_image ()
        {
            image.reset();

            int l = gui::metrics::line::width;

            XY size;
            for (auto video : mediae::selected_video)
            size.x = max (size.x, video.location.size_x + 6*l);
            size.y = image.height(size.x);

            refresh_image(size);
        }

        void refresh_image (XY size)
        {
            int maxwidth = coord.now.size.x * 2/3;
            if (maxwidth < size.x) size = XY (
                maxwidth, image.height(maxwidth));

            text.margin_right = size;
            int d = text.scroll.y.alpha.to == 0 ?
                0 : text.scroll.y.coord.now.w;

            image.coord = XYWH(
                coord.now.size.x - size.x - d, 0,
                size.x, size.y
            );
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                text.coord = coord.now.local();
                refresh_image(image.coord.now.size);
            }
            if (what == &skin)
            {
                text.view.canvas.color = gui::skins[skin].ultralight.first;
            }
            if (what == &image)
            {
                text.margin_right = image.coord.now.size;
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &text ) { clicked = text .clicked; notify(); }
            if (what == &image) { clicked = image.clicked; notify(); }
        }
    };
}

