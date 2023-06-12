#pragma once
#include "studia_l.h"
#include "studia_r.h"

struct studias:
widget<studias>
{
    studia_l studia_l;
    studia_r studia_r;
    gui::splitter splitter;

    void reload ()
    {
        studia_l.reload();
        studia_r.reload();
    }

    void on_change (void* what) override
    {
        if (what == &splitter
        or  what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int x = splitter.set("studio::splitter", 25, 55, 75);

            studia_l.coord = xyxy(0, 0, x, H);
            studia_r.coord = xyxy(x, 0, W, H);
        }
    }
};

