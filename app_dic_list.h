#include "app.h"
namespace app::dic::list
{
    auto font = [](){ return sys::font{"Segoe UI", gui::metrics::text::height*104/100}; };

    struct list : gui::widget<list>
    {
        const int word_changed = 0; // notification
        const int word_choosed = 1; // notification

        gui::widgetarium<gui::button> words;
        gui::property<int> current = 1;
        gui::property<int> origin = -1;
        gui::schema skin_middle;
        gui::schema skin_edge;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                words.coord = coord.now.local();

                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = sys::metrics(font()).height;
                int n = H / h;

                for (int i=0; i<n; i++) {
                    auto & word = i < words.size() ? words(i) : words.emplace_back();
                    word.kind = gui::button::sticky;
                    word.coord = XYWH(0, h*i, W, h);
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
                    word.text.font = font();
                    word.text.alignment = XY(gui::text::left, gui::text::center);
                    word.on_change_state = [&word, edge = i == 0 || i == words.size()-1]()
                    {
                        auto style = gui::skins[word.skin.now];
                        auto colors = style.light;

                        if (!edge) colors.back_color = style.white;

                        if (word.on           .now) colors = style.active;  else
                        if (word.mouse_hover  .now) colors = style.light;   else
                        if (word.mouse_pressed.now) colors = style.touched; else
                        if (word.enter_pressed.now) colors = style.touched; else
                        {}
                        // '=' instead of 'go' for smooth mouse passage
                        word.text.canvas.color = colors.back_color;
                        word.text.color = colors.fore_color;

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

        void refresh ()
        {
            int l = words.size()-1;
            if (l >= 2 && current.now == 0) { origin.now--; current.now = 0+1; }
            if (l >= 2 && current.now == l) { origin.now++; current.now = l-1; }

            origin.now = clamp<int>(origin.now, -1, eng::vocabulary.size() - l);

            for (int i=0; i<words.size(); i++)
            {
                int n = origin.now + i;
                bool in = 0 <= n && n < eng::vocabulary.size();
                words(i).text.text = in ? " " + eng::vocabulary[n] : "";
                words(i).enabled = in;
                words(i).on = i == current.now;
            }

            notify(word_changed);
        }

        bool mouse_sensible (XY) override { return true; }
        void on_mouse_wheel (XY p, int delta) override
        {
            origin = origin.now - delta/120;
        }

        void on_notify (gui::base::widget* w, int n) override
        {
            if (w == &words &&
                words(n).on.now &&
                words(n).mouse_pressed.now)
            {
                current = n; notify(word_choosed);
            }
        }
    };

    struct area : gui::widget<area>
    {
        gui::area<list> list;
        gui::area<gui::text::editor> word;
        gui::canvas tool;
        gui::button up, down, page_up, page_down;

        void reload ()
        {
            list.object.refresh();
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = sys::metrics(font()).height;
                int l = gui::metrics::line::width*3;
                int n = (H-2*l) / h;
                int y = 0;

                list.coord = XYWH(0, 0, W, h*(n-4) + 2*l); y += list.coord.now.size.y;
                word.coord = XYWH(0, y, W, h*3/2); y += word.coord.now.size.y;
                tool.coord = XYXY(0, y, W, H);
            }
            if (what == &skin)
            {
                word.object.background.color = gui::skins[skin.now].white;
                word.object.view.color = gui::skins[skin.now].black;
                tool.color = gui::skins[skin.now].light.back_color;
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
            if (key == "enter") on_notify(&list, l.word_choosed); else

            word.object.on_key_pressed(key,down);
        }

        void on_notify (gui::base::widget* w, int what) override
        {
            if (w == &list)
            {
                int n = list.object.origin.now + list.object.current.now;
                bool in = 0 <= n && n < eng::vocabulary.size(); if (!in) return;
                if (what == list.object.word_changed) word.object.view.text = eng::vocabulary[n];
                if (what == list.object.word_choosed) notify(n);
            }
        }
    };
}

