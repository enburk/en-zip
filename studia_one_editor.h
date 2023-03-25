#pragma once
#include "app_one.h"
namespace studio::one
{
    struct editor:
    widget<editor>
    {
        gui::text::editor editor;
        gui::binary_property<path> path;
        gui::property<gui::time> timer;
        gui::time edittime;

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go(gui::time::infinity,
                         gui::time::infinity);

            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                editor.coord = coord.now.local();
            }

            if (what == &skin)
            {
                auto& schema = gui::skins[skin];

                editor.virtual_space = true;
                editor.view.wordwrap = false;
                editor.scroll.x.mode = gui::scroll::mode::none;
                editor.view.current_line_frame.color = schema.soft.first;
                editor.padding = xyxy{gui::metrics::line::width*2,0,0,0};
                editor.font = pix::font{"Consolas"};
            }

            if (what == &path)
            {
                editor.hide(path == std::filesystem::path{});
                editor.model = doc::text::repo::load<doc::text::model>(path);
                editor.update_text = true;
            }

            if (what == &editor.update_text)
            {
                edittime = gui::time::now;
                doc::text::repo::edit(path);
                notify();
            }
            if (what == &timer and (gui::time::now - edittime) > 30s)
            {
                edittime = gui::time::infinity;
                doc::text::repo::save();
            }
        }
    };
}
