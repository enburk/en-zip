#pragma once
#include "app_dic_left.h"
#include "app_dic_list.h"
namespace app::dic
{
    struct app:
    widget<app>
    {
        left::area left;
        list::area list;
        gui::splitter splitter;

        app () { reload(); }

        void reload () try
        {
            left.current_entry = ::eng::dictionary::entry{};
            left.current_index = ::eng::dictionary::index{};
            left.reload();
            list.reload();
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int w = 10*h; // list width
                int d = 2*l;

                splitter.lower = W * 50'00 / 100'00;
                splitter.upper = W * 90'00 / 100'00;

                str s = "app::dic::splitter.permyriad";
                int p = sys::settings::load(s, 100'00 * (W-w)/W);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower.now,
                splitter.upper.now);

                splitter.coord = xyxy(x-d, 0, x+d, H);

                left.coord = xyxy(0, 0, x, H);
                list.coord = xyxy(x, 0, W, H);
            }

            if (what == &left) focus = &list;
            if (what == &left) list.select(left.clicked);
            if (what == &list) left.select(list.clicked);
            if (what == &splitter) {
                sys::settings::save(
                "app::dic::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }
        }

        void on_key(str key, bool down, bool input) override
        {
            if (input
            or  key == "tab"
            or  key == "left"
            or  key == "right"
            or  key == "up"
            or  key == "down"
            or  key == "enter"
            or  key == "escape"
            or  key == "delete"
            or  key == "backspace"
            or  key == "shift+insert"
            or  key == "ctrl+V")
                focus = &list;

            if (key == "ctrl+C"
            or  key == "ctrl+insert")
            if((left.quot.object.players.size() == 0
            or  left.quot.object.players(0).text.view.selected() == "")
            and left.card.object.text.view.selected() == "")
                focus = &list;

            if (focus.now)
                focus.now->on_key(
                key, down, input);
        }

        void on_mouse_click_child (xy p, str button, bool down) override
        {
            if (not down)
            if((left.quot.object.players.size() == 0
            or  left.quot.object.players(0).text.view.selected() == "")
            and left.card.object.text.view.selected() == "")
                focus = &list;
        }
    };
}

