#pragma once
#include "studio_app_dic_area.h"
namespace studio::dic
{
    struct studio:
    widget<studio>
    {
        gui::area<area> log;
        app::dic::app app; // after log
        gui::splitter splitter;

        studio () { reload(); }

        void reload ()
        {
            timing t0;
            locations.clear();
            dat::in::pool pool("../data/app_dict/locationary.dat");
            while (not pool.done()) {
                int source = pool.get_int();
                int offset = pool.get_int();
                str path = pool.get_string();
                locations[source][offset] = path;
            }

            timing t1;
            app::dic::logs::times << gray(monospace(
            "studio locationary " + format(t1-t0) + " sec<br>" +
            "studio load total  " + format(t1-t0) + " sec"));
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int l = gui::metrics::line::width*3;
                int w = W/2;
                int d = 2*l;

                splitter.lower = W * 25'00 / 100'00;
                splitter.upper = W * 75'00 / 100'00;

                str s = "studio::dic::splitter.permyriad";
                int p = sys::settings::load(s, 100'00 * (W-w)/W);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower.now,
                splitter.upper.now);

                splitter.coord = XYXY(x-d, 0, x+d, H);

                log.coord = XYXY(0, 0, x, H);
                app.coord = XYXY(x, 0, W, H);
            }

            if (what == &splitter) {
                sys::settings::save(
                "studio::dic::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }
        }
    };
}
