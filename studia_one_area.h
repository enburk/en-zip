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
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int l = gui::metrics::line::width;

                splitter.lower = W * 10'00 / 100'00;
                splitter.upper = W * 50'00 / 100'00;
                str s = "studio::one::area::splitter.permyriad";
                int p = sys::settings::load(s, 25'00);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower, splitter.upper);
                splitter.coord = xyxy(x-10*l, 0, x+10*l, H);

                contents.coord = xyxy(0, 0, x, H);
                editor  .coord = xyxy(x, 0, W, H);
                editor  .show_focus = true;
            }

            if (what == &splitter) {
                sys::settings::save(
                "studio::one::area::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                coord.was.size = xy{};
                on_change(&coord);
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
