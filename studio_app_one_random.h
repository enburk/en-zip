#pragma once
#include "app_one.h"
namespace studio::one
{
    struct random:
    widget<random>
    {
        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*13/10;
                int l = gui::metrics::line::width;
            }
            if (what == &skin)
            {
            }
        }
    };
}
