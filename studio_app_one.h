#pragma once
#include "studio_app_one_area.h"
namespace studio::one
{
    struct studio:
    widget<studio>
    {
        area area;
        gui::area<app::one::app> app_; // after area
        app::one::app& app = app_.object;
        gui::splitter splitter;

        studio () { reload(); }

        void reload () {}

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int d = gui::metrics::line::width*10;

                splitter.lower = W * 25'00 / 100'00;
                splitter.upper = W * 75'00 / 100'00;
                str s = "studio::one::splitter.permyriad";
                int p = sys::settings::load(s, 40'00);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower, splitter.upper);
                splitter.coord = xyxy(x-d, 0, x+d, H);

                area.coord = xyxy(0, 0, x, H);
                app_.coord = xyxy(x, 0, W, H);
            }

            if (what == &splitter) {
                sys::settings::save(
                "studio::one::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }
        }
    };
}

