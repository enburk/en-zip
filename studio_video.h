#pragma once
#include "app.h"
namespace studio::video
{
    struct studio : gui::widget<studio>
    {
        gui::canvas canvas;

        studio ()
        {
            canvas.color = gui::skins[skin.now].light.first;
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;

                canvas.coord = XYWH(0, 0, W, H);
            }
        }
    };
}

