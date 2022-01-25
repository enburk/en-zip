#pragma once
#include "app.h"
namespace app::dic::list
{
    struct word:
    widget<word>
    {
        gui::text::one_line_editor editor;
        
        int typed = 0;

        bool flag = false;

        void select (int n)
        {
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
                editor.virtual_space = true;
                editor.rpadding.now = gui::metrics::text::height/6;
                editor.canvas.color = gui::skins[skin].ultralight.first;
                editor.style = pix::text::style{
                    sys::font{"Segoe UI",
                    gui::metrics::text::height},
                    gui::skins[skin].dark.first };
            }
            if (what == &editor) if (not flag)
            {
                if (vocabulary.size() == 0) return;
                auto s = editor.text.now; s.triml();
                auto i = vocabulary.lower_bound_case_insensitive(s);

                editor.style = pix::text::style{
                    sys::font{"Segoe UI",
                    gui::metrics::text::height},
                    eng::equal_case_insensitive(s, vocabulary[i].title) ?
                    gui::skins[skin].dark.first :
                    gui::skins[skin].error.first };

                typed = i;
                notify();
            }
        }

        void on_focus (bool on) override { editor.on_focus(on); }
        void on_key_input (str symbol) override { editor.on_key_input(symbol); }
        void on_key_pressed (str key, bool down) override
        {
            if (down and
               (key == "ctrl+V" or
                key == "shift+insert"))
            {
                str s =
                sys::clipboard::get::string();
                s.replace_all("\t", " ");
                s.replace_all("\n", " ");
                s.replace_all("\r", "");
                s.strip();
                sys::clipboard::set(s);
            }
            editor.on_key_pressed(key,down);
        }
    };
}
