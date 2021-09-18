#pragma once
#include "app.h"
#include "app_dic_list_list.h"
#include "app_dic_list_word.h"
namespace app::dic::list
{
    struct area:
    widget<area>
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
            using namespace std::literals::chrono_literals;
            auto lapse = 40ms;
            up.repeat_delay = 0ms;
            up.repeat_lapse = lapse;
            down.repeat_delay = 0ms;
            down.repeat_lapse = lapse;
            page_up.repeat_delay = 0ms;
            page_up.repeat_lapse = lapse;
            page_down.repeat_delay = 0ms;
            page_down.repeat_lapse = lapse;
        }

        void reload ()
        {
            list.object.refresh();
            up       .icon.load(assets["icon.chevron.up.black.128x128"]);
            down     .icon.load(assets["icon.chevron.down.black.128x128"]);
            page_up  .icon.load(assets["icon.chevron.up.double.black.128x128"]);
            page_down.icon.load(assets["icon.chevron.down.double.black.128x128"]);
            settings .icon.load(assets["icon.settings.black.192x192"]);
        }

        void select (int n)
        {
            /// if (log) *log <<
            /// "app::dict::list::select "
            /// + std::to_string(n) + " "
            /// + vocabulary[n].title;

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

                int hword = h*14/7;
                int htool = h*12/7;
                int hlist = H - hword - htool;
                int w = min (htool, W/5);
                int y = 0;

                list.coord = XYWH(0, 0, W, hlist); y += list.coord.now.size.y;
                word.coord = XYWH(0, y, W, hword); y += word.coord.now.size.y;
                tool.coord = XYXY(0, y, W, H);

                int l = w/5;
                up       .icon.padding = XYXY(l,l,l,l);
                down     .icon.padding = XYXY(l,l,l,l);
                page_up  .icon.padding = XYXY(l,l,l,l);
                page_down.icon.padding = XYXY(l,l,l,l); l = w/7;
                settings .icon.padding = XYXY(l,l,l,l);

                up       .coord = XYXY(W-5*w, y, W-4*w, H);
                down     .coord = XYXY(W-4*w, y, W-3*w, H);
                page_up  .coord = XYXY(W-3*w, y, W-2*w, H);
                page_down.coord = XYXY(W-2*w, y, W-1*w, H);
                settings .coord = XYXY(W-1*w, y, W-0*w, H);
            }
            if (what == &skin)
            {
                tool.color = gui::skins[skin].light.first;
                auto font = sys::font{"", gui::metrics::text::height*12/10};
                up       .text.font = font;
                down     .text.font = font;
                page_up  .text.font = font;
                page_down.text.font = font;
                settings .text.font = font;
            }
        }

        void on_focus (bool on) override { word.object.on_focus(on); }
        void on_keyboard_input (str symbol) override { word.object.on_keyboard_input(symbol); }
        void on_key_pressed (str key, bool down) override
        {
            auto & l = list.object;
            int page = l.words.size();
            if (sys::keyboard::shift) page *= list.object.words.size();
            if (sys::keyboard::ctrl) page *= 5;

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
