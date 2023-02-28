#pragma once
#include "app.h"
namespace app::one
{
    struct entry:
    widget<entry>
    {
        enum class state {
        init, wait, load, read,
        fade, show, done, hide, gone};

        state status;
        where where; Entry entry; AudioItem texto; VideoItem image; XYWH coords; bool was_pause = false, auto_show = false, fade_anyway = false;

        gui::property<gui::time> timer;

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

