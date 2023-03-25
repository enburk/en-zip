#pragma once
#include "studia_one_contents.h"
#include "studia_one_editor.h"
namespace studio::one
{
    struct area:
    widget<area>
    {
        gui::area<contents> contents;
        gui::area<editor> editor;
        gui::splitter splitter;

        void on_change (void* what) override
        {
            if (what == &splitter
            or  what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int l = gui::metrics::line::width;
                int x = splitter.set("studio::one::area::splitter", 10, 25, 50);

                contents.coord = xyxy(0, 0, x, H);
                editor  .coord = xyxy(x, 0, W, H);
                editor  .show_focus = true;
            }

            if (what == &contents)
            {
                editor.object.path = contents.object.selected.now;
                
                if (editor.object.path.now ==
                    editor.object.path.was) {
                    doc::text::repo::reload();
                    editor.object.editor.
                    update_text = true; }
            }
        }
    };
}
