#pragma once
#include "app.h"
namespace app::dict::list
{
    struct list : gui::widget<list>
    {
        gui::radio::group words;
        gui::property<int> current = 1;
        gui::property<int> origin = -1;
        int selected = 0;

        enum class note { chosen, changed }; note note;

        bool flag = false;

        void select (int n)
        {
            /// if (log) *log <<
            /// "app::dict::list::list::select "
            /// + std::to_string(n);

            if (n < 0) return;
            if (n >= vocabulary.size()) return;
            origin.now = n - current.now;
            current.now = n - origin.now;
            flag = true;
            refresh();
            flag = false;
        }

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
            if (what == &origin) refresh();
            if (what == &current) refresh();
            if (what == &selected) refresh();
            if (what == &selected) refresh();
        }

        void refresh ()
        {
            if (words.size() == 0) return;
            if (vocabulary.size() == 0) return;

            int l = words.size()-1;
            if (l >= 2 && current.now == 0) { origin.now--; current.now++; }
            if (l >= 2 && current.now == l) { origin.now++; current.now--; }

            origin.now = clamp<int>(origin.now, -1, vocabulary.size() - l);

            for (int i=0; i<words.size(); i++) words(i).on = false;
            for (int i=0; i<words.size(); i++)
            {
                int n = origin.now + i;
                bool in = 0 <= n && n < vocabulary.size();
                words(i).text.text = in ? " " + vocabulary[n].title : "";
                words(i).enabled = in;
            }

            words(current.now).on = true;

            selected = origin.now + current.now;
            note = note::changed;
            if (not flag) notify();
        }

        void on_notify (void* what) override
        {
            if (what == &words) if (not flag)
            {
                flag = true;
                current = words.notifier_index;
                flag = false;
                note = note::chosen;
                notify();
            }
        }
    };
}
