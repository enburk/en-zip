#pragma once
#include "app_dic_left.h"
#include "app_dic_list.h"
namespace app::dic
{
    struct view:
    widget<view>
    {
        left::area left;
        list::area list;
        gui::splitter splitter;

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
            if (what == &splitter
            or  what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int x = splitter.set("app::dic::splitter", 50, 85, 95);

                left.coord = xyxy(0, 0, x, H);
                list.coord = xyxy(x, 0, W, H);
            }

            if (what == &left) focus = &list;
            if (what == &left) list.select(left.clicked);
            if (what == &list) left.select(list.clicked);
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
            if (not left.quot.object.text_selected()
            and not left.card.object.text_selected())
                focus = &list;

            if (focus.now)
                focus.now->on_key(
                key, down, input);
        }

        void on_mouse_click_child (xy p, str button, bool down) override
        {
            if (not down
            and not left.quot.object.text_selected()
            and not left.card.object.text_selected())
                focus = &list;
        }
    };
}

