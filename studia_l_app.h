#pragma once
#include "studia_l_app_contents.h"
#include "studia_l_app_editor.h"
namespace studia
{
    struct content:
    widget<content>
    {
        binary_property<path> root;
        gui::area<gui::text::view> where;
        gui::area<contents> contents;
        gui::area<editor> editor;
        gui::splitter splitter;

        void on_change (void* what) override
        {
            if (what == &splitter
            or  what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height*13/10;
                int l = gui::metrics::line::width;
                int x = splitter.set(name.now +
                     "::splitter", 10, 25, 50);

                contents.coord = xyxy(0, 0, x, H);
                where   .coord = xyxy(x, 0, W, h);
                editor  .coord = xyxy(x, h, W, H);
                editor  .show_focus = true;
            }

            if (what == &name)
                contents.object.
                name =
                name.now +
                "::contents";

            if (what == &root)
                contents.object.
                root =
                root;

            if (what == &contents)
            {
                editor.object.path = contents.object.selected.now;
                
                if (editor.object.path.now ==
                    editor.object.path.was) {
                    doc::text::repo::reload();
                    editor.object.editor.
                    update_text = true; }

                path p =
                std::filesystem::relative(editor.object.path,
                std::filesystem::current_path());

                str
                header(p);
                header.replace_all("\\", "/");
                header.resize(header.size()-4); // .txt
                array<str> ss = header.split_by("/");
                ss.upto(1).erase(); // content/
                for (str& s: ss) {
                s = s.from(3);
                if (s.starts_with("''")
                and s.  ends_with("''")) {
                    s.truncate(); s.erase(0);
                    s.truncate(); s.erase(0);
                    s = extracolor(
                    s); }
                }
                header = str(ss, blue("/"));
                where.object.html = header;
            }
        }
    };
}
