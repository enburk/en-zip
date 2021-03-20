#pragma once
#include "app.h"
namespace app::dict::list
{
    struct list : gui::widget<list>
    {
        enum class note { chosen, changed }; note note;

        gui::widgetarium<gui::button> words;
        gui::property<int> current = 1;
        gui::property<int> origin = -1;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                words.coord = coord.now.local();

                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height*99/70;
                int n = (H + h - 1) / h;

                for (int i=0; i<n; i++) {
                    auto & word = i < words.size() ? words(i) : words.emplace_back();
                    word.kind = gui::button::sticky;
                    word.coord = XYWH(0, h*i, W, h);
                    word.text.word_wrap = false;
                    word.text.ellipsis = true;
                }
                words.truncate(n);
                on_change(&skin);
                refresh();
            }
            if (what == &skin)
            {
                for (int i=0; i<words.size(); i++)
                {
                    auto & word = words(i);
                    word.text.alignment = XY(pix::left, pix::center);
                    word.on_change_state = [&word, edge = i == 0 || i == words.size()-1]()
                    {
                        auto style = gui::skins[word.skin.now];
                        auto colors = style.light;

                        if (!edge) colors.first = style.ultralight.first;
                        // order important
                        if (word.mouse_pressed.now) colors = style.touched; else
                        if (word.enter_pressed.now) colors = style.touched; else
                        if (word.on           .now) colors = style.active;  else
                        if (word.mouse_hover  .now) colors = style.light;   else
                        {}
                        // '=' instead of 'go' for smooth mouse passage
                        word.text.canvas.color = colors.first;
                        word.text.color = colors.second;

                        auto r = word.coord.now.local();
                        word.frame.thickness = 0;
                        word.frame.coord = XYWH();
                        word.image.coord = XYWH();
                        word.text .coord = r;
                    };
                    word.on_change_state();
                }
            }
            if (what == &current) refresh();
            if (what == &origin) refresh();
        }

        int clicked = 0;

        void refresh ()
        {
            int l = words.size()-1;
            if (l >= 2 && current.now == 0) { origin.now--; current.now = 0+1; }
            if (l >= 2 && current.now == l) { origin.now++; current.now = l-1; }

            origin.now = clamp<int>(origin.now, -1, vocabulary.size() - l);

            for (int i=0; i<words.size(); i++)
            {
                int n = origin.now + i;
                bool in = 0 <= n && n < vocabulary.size();
                words(i).text.text = in ? " " + vocabulary[n].title : "";
                words(i).enabled = in;
                words(i).on = i == current.now;
            }

            note = note::changed;
            notify();
        }

        void on_notify (void* what) override
        {
            if (what == &words &&
                words.notifier->on.now &&
                words.notifier->mouse_pressed.now)
            {
                current = words.notifier_index;
                note = note::chosen;
                notify();
            }
        }
    };

    struct area : gui::widget<area>
    {
        gui::area<list> list;
        gui::area<gui::text::one_line_editor> word;

        gui::canvas tool;
        gui::button settings;
        gui::button up, down;
        gui::button page_up, page_down;

        area ()
        {
            up.text.text = (char*)(u8"\u2191");
            down.text.text = (char*)(u8"\u2193");
            up.repeating = true;
            down.repeating = true;
            page_up.text.text = (char*)(u8"\u21C8");
            page_down.text.text = (char*)(u8"\u21CA");
            page_up.repeating = true;
            page_down.repeating = true;
            settings.text.text = (char*)(u8"\u26ED");
        }

        void reload ()
        {
            list.object.refresh();
            list.object.note = list::note::chosen;
            on_notify(&list);
        }

        void select (int n)
        {
            if (n < 0) return;
            if (n >= vocabulary.size()) return;
            list.object.origin = n - list.object.current.now;
            list.object.current = n - list.object.origin.now;
            list.object.note = list::note::chosen;
            on_notify(&list.object);
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
                word.object.margin_left = XY{gui::metrics::text::height/6, max<int>()};
                word.object.canvas.color = gui::skins[skin].ultralight.first;
                word.object.style = pix::text::style{
                    sys::font{"Segoe UI",
                    gui::metrics::text::height},
                    gui::skins[skin].dark.first };
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
                l.note = list::note::chosen;
                on_notify(&l);
            }
            else word.object.on_key_pressed(key,down);
        }

        bool flag = false;

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &word.object)
            {
                if (vocabulary.size() == 0) return;
                auto s = word.object.text.now; s.triml();
                auto i = vocabulary.lower_bound(
                    eng::vocabulary::entry{s},
                    eng::vocabulary::less);

                if (i == vocabulary.end()) i--;
                if (i->title != s) i = vocabulary.lower_bound(
                    eng::vocabulary::entry{s},
                    eng::vocabulary::less_case_insensitive);
                if (i == vocabulary.end()) i--;

                word.object.style = pix::text::style{
                    sys::font{"Segoe UI",
                    gui::metrics::text::height},
                    eng::less_case_insensitive(i->title, s) or
                    eng::less_case_insensitive(s, i->title)?
                    gui::skins[skin].error.first:
                    gui::skins[skin].dark.first };

                flag = true;
                int n = (int)(i - vocabulary.begin());
                list.object.origin = n - list.object.current.now;
                flag = false;
            }

            if (what == &list.object) if (!flag)
            {
                int n = list.object.origin.now + list.object.current.now;
                bool in = 0 <= n && n < vocabulary.size(); if (!in) return;
        
                word.object.text = vocabulary[n].title;
                word.object.select();
                
                if (list.object.note ==
                    list::note::chosen) {
                    clicked = n;
                    notify();
                }
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

