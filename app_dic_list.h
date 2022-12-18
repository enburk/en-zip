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
        int clicked = 0;

        void reload ()
        {
            list.object.refresh();
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
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;

                int hword = h*12/10;
                int hlist = H - hword;
                int y = 0;

                list.coord = xywh(0, 0, W, hlist); y += list.coord.now.size.y;
                word.coord = xywh(0, y, W, hword); y += word.coord.now.size.y;
                word.show_focus = true;
            }

            if (what == &word.object) list.object.select(word.object.typed);
            if (what == &list.object) word.object.select(list.object.selected);
            if (what == &list.object and list.object.note == list::note::chosen)
            {
                clicked = list.object.selected;
                notify();
            }
        }

        void on_key (str key, bool down, bool input) override
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
            else word.object.on_key(key, down, input);
        }

        bool mouse_sensible (xy) override { return true; }
        bool on_mouse_wheel (xy p, int delta) override
        {
            if (sys::keyboard::shift) delta *= list.object.words.size();
            if (sys::keyboard::ctrl) delta *= 5;
            list.object.origin =
            list.object.origin.now
            - delta/60;//120;
            return true;
        }
    };
}
