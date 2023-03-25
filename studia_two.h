#pragma once
#include "app_two.h"
namespace studio::two
{
    struct area:
    widget<area>
    {
        gui::console log;

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                log.coord = coord.now.local();
            }
        }
    };

    struct studio:
    widget<studio>
    {
        app::two::app app;
        gui::area<area> area;
        gui::splitter splitter;

        studio () { reload(); }

        void reload () { app.reload(); }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int l = gui::metrics::line::width*3;
                int w = W/2;
                int d = 2*l;

                splitter.coord = xywh(W-w-d, 0, 2*d, H);
                splitter.lower = 80'00 * W / 100'00;
                splitter.upper =  8'00 * W / 100'00;

                area.coord = xywh(0, 0, W-w, H);
                app .coord = xywh(W-w, 0, w, H);
            }
        }
    };
}

