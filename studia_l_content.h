#pragma once
#include "studia_l_editor.h"
namespace studia
{
    struct Content:
    widget<Content>
    {
        struct contents: sfx::dirtree
        {
            contents ()
            {
                sfx::dirtree::contents.pretty = [](record r)
                {
                    str name = r.name.from(3);
                    if (name.ends_with(".txt"))
                        name.truncate(),
                        name.truncate(),
                        name.truncate(),
                        name.truncate();

                    if (name.starts_with("''")
                    and name.  ends_with("''")) {
                        name.truncate(); name.erase(0);
                        name.truncate(); name.erase(0);
                        name = extracolor(
                        name); }

                    return r.file ? name :
                        bold(name);
                };
            }
        };

        binary_property<path> root;
        gui::area<gui::text::view> where;
        gui::area<gui::button> back;
        gui::area<gui::button> fore;
        gui::area<contents> contents;
        gui::area<editor> editor;
        gui::splitter splitter;
        array<path> backs;
        array<path> fores;

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
                where   .coord = xyxy(x,     0, W-2*h, h);
                back    .coord = xyxy(W-2*h, 0, W-1*h, h);
                fore    .coord = xyxy(W-1*h, 0, W-0*h, h);
                editor  .coord = xyxy(x, h, W, H);
                editor  .show_focus = true;
            }

            if (what == &skin)
            {
                back.object.text.html = monospace(bold(u8"←"));
                fore.object.text.html = monospace(bold(u8"→"));
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
                auto target = contents.object.selected.now;

                if (editor.object.path != path{}
                and editor.object.path != target)
                if (backs.empty() or
                    backs.back() !=
                    editor.object.path)
                {
                    backs +=
                    editor.object.path;
                    fores.clear();
                }

                editor.object.path = target;
                
                if (editor.object.path.now ==
                    editor.object.path.was) {
                    doc::text::repo::reload();
                    editor.object.editor.
                    update_text = true;
                }

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

            if (what == &back and not backs.empty())
            {
                auto path =
                backs.back();
                backs.pop_back();
                fores += editor.object.path;
                auto Backs = std::move(backs);
                auto Fores = std::move(fores);
                contents.object.selected = path;
                backs = std::move(Backs);
                fores = std::move(Fores);
            }
            if (what == &fore and not fores.empty())
            {
                auto path =
                fores.back();
                fores.pop_back();
                backs += editor.object.path;
                auto Backs = std::move(backs);
                auto Fores = std::move(fores);
                contents.object.selected = path;
                backs = std::move(Backs);
                fores = std::move(Fores);
            }

            back.object.enabled = not backs.empty();
            fore.object.enabled = not fores.empty();
        }
    };
}
