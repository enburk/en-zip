#pragma once
#include "app.h"
namespace app::one
{
    struct app : gui::widget<app>
    {
        gui::canvas canvas;

        app() { reload(); }

        void reload () try
        {
        }
        catch (std::exception & e) {
        log << bold(red(e.what())); }

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
                canvas.color = gui::skins[skin.now].ultralight.first;
            }
        }
    };
}

