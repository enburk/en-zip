#pragma once
#include "app.h"
namespace app::dic::list
{
    struct list:
    widget<list>
    {
        gui::canvas canvas;
        gui::radio::group words;
        gui::property<int> current = 1;
        gui::property<int> origin = -1;
        int selected = 0;

        enum class note { chosen, changed }; note note;

        bool flag = false;

        void select (int n)
        {
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
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                words .coord = coord.now.local();
                canvas.coord = coord.now.local();

                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int n = (H + h - 1) / h;

                for (int i=0; i<n; i++) {
                    auto & word = i < words.size() ? words(i) : words.emplace_back();
                    word.kind = gui::button::sticky;
                    word.coord = xywh(0, h*i, W, h);
                    word.text.alignment = xy(pix::left, pix::center);
                    word.text.lwrap = array<xy>{{h/6,h}};
                    word.text.rwrap = array<xy>{{h/6,h}};
                    word.text.ellipsis = true;
                }
                words.truncate(n);
                on_change(&skin);
                refresh();
            }
            if (what == &skin)
            {
                canvas.color = gui::skins[skin].light.first;

                for (int i=0; i<words.size(); i++)
                {
                    auto & word = words(i);
                    word.on_change_state = [&word, edge = i == 0 or i == words.size()-1]()
                    {
                        auto style = gui::skins[word.skin.now];
                        auto colors = style.light;

                        if (!edge) colors.first = style.ultralight.first;
                        // order important
                        if (word.mouse_clicked.now) colors = style.touched; else
                        if (word.enter_pressed.now) colors = style.touched; else
                        if (word.on           .now) colors = style.active;  else
                        if (word.mouse_hover  .now) colors = style.light;   else
                        {}
                        word.canvas.color = colors.first;
                        word.text.color = colors.second;

                        auto r = word.coord.now.local();
                        word.frame.thickness = 0;
                        word.frame.coord = xywh{};
                        word.image.coord = xywh{};
                        word.text.coord = r; r.deflate(1);
                        word.canvas.coord = r;
                    };
                    word.on_change_state();
                }
            }
            if (what == &origin) refresh();
            if (what == &current) refresh();
            if (what == &selected) refresh();
            if (what == &words) if (not flag)
            {
                flag = true;
                current = words.notifier_index;
                flag = false;
                note = note::chosen;
                notify();
            }
        }

        void refresh ()
        {
            if (words.size() == 0) return;
            if (vocabulary.size() == 0) return;

            int l = words.size()-1;

            if (l >= 2 and current.now == 0) { origin.now--; current.now++; }
            if (l >= 2 and current.now == l) { origin.now++; current.now--; }

            if (origin.now > vocabulary.size()-l) { current.now +=
                origin.now -(vocabulary.size()-l);
                origin.now = vocabulary.size()-l;
            }
            if (origin.now < -1) { current.now +=
                origin.now -(-1);
                origin.now = -1;
            }
            if (current.now > l)
                current.now = l;
            if (current.now > vocabulary.size()-1 - origin.now)
                current.now = vocabulary.size()-1 - origin.now;
            if (current.now < 0)
                current.now = 0;

            for (int i=0; i<words.size(); i++) words(i).on = false;
            for (int i=0; i<words.size(); i++)
            {
                int n = origin.now + i;
                bool in = 0 <= n && n < vocabulary.size();
                words(i).text.text = in ? vocabulary[n].title : "";
                words(i).enabled = in;
            }

            bool
            f = flag; flag = true;
            words(current.now).on = true;
            flag = f;

            selected = origin.now + current.now;
            note = note::changed;
            if (not flag) notify();
        }
    };
}
