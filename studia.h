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

        if (what == &studia_l.link)
            focus = &studia_r,
            studia_r.click(
            studia_l.link);

        if (what == &studia_r.link)
            focus = &studia_l,
            studia_l.click(
            studia_r.link);

        if (what == &focus_on
            and focus_on.now
            and not focus)
            focus = &studia_r;
    }

    void on_key(str key, bool down, bool input) override
    {
        if (key == "tab" and down)
        {
            if (focus.now == &studia_l)
                focus = &studia_r; else
                focus = &studia_l;
        }
        else
        {
            if (focus.now)
                focus.now->on_key(
                key, down, input);
        }
    }
};

