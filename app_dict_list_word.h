#pragma once
#include "app.h"
namespace app::dict::list
{
    struct word : gui::widget<word>
    {
        gui::text::one_line_editor editor;
        
        int typed = 0;

        bool flag = false;

        void select (int n)
        {
            /// if (log) *log <<
            /// "app::dict::list::word::select "
            /// + std::to_string(n);

            if (n < 0) return;
            if (n >= vocabulary.size()) return;
            flag = true;
            editor.text = vocabulary[n].title;
            flag = false;

            editor.select();
            editor.style = pix::text::style{
                sys::font{"Segoe UI",
                gui::metrics::text::height},
                gui::skins[skin].dark.first};
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                editor.coord = coord.now.local();
            }
            if (what == &skin)
            {
                editor.margin_left = XY{gui::metrics::text::height/6, max<int>()};
                editor.canvas.color = gui::skins[skin].ultralight.first;
                editor.style = pix::text::style{
                    sys::font{"Segoe UI",
                    gui::metrics::text::height},
                    gui::skins[skin].dark.first };
            }
        }

        void on_focus (bool on) override { editor.on_focus(on); }
        void on_keyboard_input (str symbol) override { editor.on_keyboard_input(symbol); }
        void on_key_pressed (str key, bool down) override { editor.on_key_pressed(key,down); }

        void on_notify (void* what) override
        {
            if (what == &editor) if (not flag)
            {
                if (vocabulary.size() == 0) return;
                auto s = editor.text.now; s.triml();
                auto S = editor.text.now; S.strip();
                auto i = vocabulary.lower_bound(
                    eng::vocabulary::entry{s},
                    eng::vocabulary::less);

                if (i == vocabulary.end())
                    i =  vocabulary.lower_bound(
                    eng::vocabulary::entry{S},
                    eng::vocabulary::less);

                if (i == vocabulary.end()) i--;

                if (i->title != s)
                    i =  vocabulary.lower_bound(
                    eng::vocabulary::entry{s},
                    eng::vocabulary::less_case_insensitive);
                
                if (i == vocabulary.end())
                    i =  vocabulary.lower_bound(
                    eng::vocabulary::entry{S},
                    eng::vocabulary::less_case_insensitive);

                if (i == vocabulary.end()) i--;

                editor.style = pix::text::style{
                    sys::font{"Segoe UI",
                    gui::metrics::text::height},
                    eng::equal_case_insensitive(s, i->title)?
                    gui::skins[skin].dark.first:
                    gui::skins[skin].error.first};

                typed = (int)(i - vocabulary.begin());
                notify();
            }
        }
    };
}
