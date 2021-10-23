#pragma once
#include "app_dic.h"
#include "eng_parser.h"
#include "eng_unittest.h"
#include "media_data.h"
namespace studio::dic
{
    struct area : gui::widget<area>
    {
        gui::console log;

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                log.coord = coord.now.local();
            }
        }
    };

    struct studio : gui::widget<studio>
    {
        app::dic::app app;
        gui::area<area> area;
        gui::splitter splitter;

        studio () { app::dic::log = area.object.log; }

        void reload () { app.reload(); }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int l = gui::metrics::line::width*3;
                int w = W/2;
                int d = 2*l;

                splitter.coord = XYWH(W-w-d, 0, 2*d, H);
                splitter.lower = 8'000 * W / 10'000;
                splitter.upper =   800 * W / 10'000;

                area.coord = XYWH(0, 0, W-w, H);
                app .coord = XYWH(W-w, 0, w, H);
            }
        }

        void on_focus (bool on) override { app.on_focus(on); }
        void on_keyboard_input (str symbol) override
        {
            area.object.log.page.view.selections = array<gui::text::range>();
            app.on_keyboard_input(symbol);
        }
        void on_key_pressed (str key, bool down) override
        {
            if (key == "") return;
            if((key == "ctrl+C" or
                key == "ctrl+insert" or
                key == "shift+left"  or
                key == "shift+right" or
                key == "ctrl+left"   or
                key == "ctrl+right"  or
                key == "ctrl+shift+left"  or
                key == "ctrl+shift+right" or
                key == "shift+up"    or
                key == "shift+down") and
                area.object.log.page.view.selected() != "") {
                area.object.log.page.on_key_pressed(key,down);
                return;
            }

            area.object.log.page.view.selections = array<gui::text::range>();
            app.on_key_pressed(key,down);
        }
    };
}
