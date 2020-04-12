#pragma once
#include "app.h"
namespace app::mono
{
    struct app : gui::widget<app>
    {
        gui::canvas canvas;

        app() { reload(); }

        void reload ()
        {
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;

                canvas.coord = XYWH(0, 0, W, H);
            }
            if (what == &skin)
            {
                canvas.color = gui::skins[skin.now].light.back_color;
            }
        }
    };
}

