#pragma once
#include "app.h"
namespace app::two
{
    struct view:
    widget<view>
    {
        gui::canvas canvas;

        void reload ()
        {
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;

                canvas.coord = xywh(0, 0, W, H);
            }
            if (what == &skin)
            {
                canvas.color = gui::skins[skin.now].light.first;
            }
        }
    };
}

