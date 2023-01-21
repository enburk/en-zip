#pragma once
#include "studio_app_one_area.h"
#include "studio_app_one_preview.h"
namespace studio::one
{
    struct studio:
    widget<studio>
    {
        area left;
        app::dic::app dic;
        app::one::app app;
        gui::area<preview> preview;
        array<gui::base::widget*> right;
        gui::area<gui::selector> selector;
        gui::splitter splitter;

        studio ()
        {
            reload();

            right += &dic;
            right += &preview;
            right += &app;

            for (int i=1; i<
            right.size(); i++)
            right[i]->hide();

            int i = 0;
            selector.object.buttons(i++).text.text = "dictionary";
            selector.object.buttons(i++).text.text = "preview";
            selector.object.buttons(i++).text.text = "app";
            selector.object.maxwidth = max<int>();
            selector.object.selected = 0;
        }

        void reload () { app.reload(); }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*13/10;
                int l = gui::metrics::line::width;

                splitter.lower = W * 25'00 / 100'00;
                splitter.upper = W * 75'00 / 100'00;
                str s = "studio::one::splitter.permyriad";
                int p = sys::settings::load(s, 40'00);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower, splitter.upper);
                splitter.coord = xyxy(x-10*l, 0, x+10*l, H);

                selector.coord = xywh(0, 0, x, h);
                left    .coord = xyxy(0, h, x, H);
                dic     .coord = xyxy(x, 0, W, H);
                app     .coord = xyxy(x, 0, W, H);
                preview .coord = xyxy(x, 0, W, H);
            }

            if (what == &splitter) {
                sys::settings::save(
                "studio::one::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }

            if (what == &selector)
            {
                int n = selector.object.selected.now;
                for (int i=0; i<right.size(); i++)
                right[i]->show(i == n);
            }
        }

        void on_key(str key, bool down, bool input) override
        {
            if (key == "tab" and down)
            {
                if (focus.now != &left)
                    focus = &left; else
                    for (auto& r: right)
                    if (r->alpha.now == 255)
                    focus = r;
            }

            if (focus.now)
                focus.now->on_key(
                key, down, input);
        }
    };
}

