#pragma once
#include "app.h"
#include "app_one_entry.h"
namespace app::one
{
    struct slide:
    widget<slide>
    {
        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
            }
        }
    };
}