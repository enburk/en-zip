#pragma once
#include "app.h"
#include "app_dict_list_list.h"
#include "app_dict_list_word.h"
namespace app::dict::list
{
    struct area : gui::widget<area>
    {
        gui::area<list> list;
        gui::area<word> word;

        gui::canvas tool;
        gui::button settings;
        gui::button up, down;
        gui::button page_up, page_down;

        area ()
        {
            up.repeating = true;
            down.repeating = true;
            page_up.repeating = true;
            page_down.repeating = true;
            up.text.text = (char*)(u8"\u2191");
            down.text.text = (char*)(u8"\u2193");
            page_up.text.text = (char*)(u8"\u21C8");
            page_down.text.text = (char*)(u8"\u21CA");
            settings.text.text = (char*)(u8"\u26ED");
        }

        void reload ()
        {
            if (true) if (log) *log <<
            "app::dict::list::reload";

            list.object.refresh();
        }

        void select (int n)
        {
            if (true) if (log) *log <<
            "app::dict::list::select "
            + std::to_string(n);

            list.object.select(n);
            word.object.select(list.object.selected);
            clicked = list.object.selected;
            notify();
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;

                int hword = h*13/7;
                int htool = h*13/7;
                int hlist = H - hword - htool;
                int w = min (htool, W/5);
                int y = 0;

                list.coord = XYWH(0, 0, W, hlist); y += list.coord.now.size.y;
                word.coord = XYWH(0, y, W, hword); y += word.coord.now.size.y;
                tool.coord = XYXY(0, y, W, H);

                up       .coord = XYXY(W-5*w, y, W-4*w, H);
                down     .coord = XYXY(W-4*w, y, W-3*w, H);
                page_up  .coord = XYXY(W-3*w, y, W-2*w, H);
                page_down.coord = XYXY(W-2*w, y, W-1*w, H);
                settings .coord = XYXY(W-1*w, y, W-0*w, H);
            }
            if (what == &skin)
            {
                tool.color = gui::skins[skin].light.first;
            }
        }

        void on_focus (bool on) override { word.object.on_focus(on); }
        void on_keyboard_input (str symbol) override { word.object.on_keyboard_input(symbol); }
        void on_key_pressed (str key, bool down) override
        {
            auto & l = list.object;
            int page = l.words.size();

            if (!down) return;
            if (key == "up"  ) l.current = l.current.now - 1; else
            if (key == "down") l.current = l.current.now + 1; else
            if (key == "ctrl+page up"  ) l.current = 0; else
            if (key == "ctrl+page down") l.current = page-1; else
            if (key == "ctrl+up"  ) l.origin = l.origin.now - 1; else
            if (key == "ctrl+down") l.origin = l.origin.now + 1; else
            if (key == "page up"  ) l.origin = l.origin.now - page; else
            if (key == "page down") l.origin = l.origin.now + page; else
            if (key == "ctrl+home") l.origin = -1; else
            if (key == "ctrl+end" ) l.origin = max<int>(); else
            if (key == "enter")
            {
                word.object.select(l.selected);
                clicked = l.selected;
                notify();
            }
            else word.object.on_key_pressed(key,down);
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &word.object) list.object.select(word.object.typed);
            if (what == &list.object) word.object.select(list.object.selected);
            if (what == &list.object) if (list.object.note == list::note::chosen)
            {
                clicked = list.object.selected;
                notify();
            }

            if (what == &up       ) on_key_pressed("up"  , true);
            if (what == &down     ) on_key_pressed("down", true);
            if (what == &page_up  ) on_key_pressed("page up"  , true);
            if (what == &page_down) on_key_pressed("page down", true);
            if (what == &settings ) {}
        }

        bool mouse_sensible (XY) override { return true; }
        bool on_mouse_wheel (XY p, int delta) override
        {
            if (sys::keyboard::shift) delta *= list.object.words.size();
            if (sys::keyboard::ctrl) delta *= 5;
            list.object.origin =
            list.object.origin.now
            - delta/20;//120;
            return true;
        }
    };
}
