#pragma once
#include "app_one.h"
namespace studia
{
    struct contents:
    widget<contents>
    {
        gui::table table;
        gui::area<gui::selector> selector;
        binary_property<path> root;
        sfx::dirwatcher watcher;
        bool extra = false;
        str link;

        contents ()
        {
            int i = 0;
            auto& select = selector.object;
            select.buttons(i++).text.text = "Elementary";
            select.buttons(i++).text.html = extracolor("Extra");
            select.buttons(i++).text.text = "Intermediate 1";
            select.buttons(i++).text.html = extracolor("Extra");
            select.buttons(i++).text.text = "Intermediate 2";
            select.buttons(i++).text.html = extracolor("Extra");
            select.buttons(i++).text.text = "Advanced";
            select.buttons(i++).text.html = extracolor("Extra");
            select.selected = 0;
        }

        void replane ()
        {
            table.refresh();
        }

        void fill ()
        {
            table.cells.clear();

            auto on_change_state = [](gui::button& b)
            {
                auto style = gui::skins[b.skin.now];
                auto r =
                b.coord.now.local();
                b.frame.thickness = 0;
                b.frame.coord = xywh{};
                b.image.coord = xywh{};
                b.text.coord = r; r.deflate(1);
                b.canvas.coord = r;
                b.canvas.color =
                b.enabled.now ?
                b.mouse_hover.now?
                b.mouse_clicked.now?
                style.middle.first :
                style.normal.first :
                rgba{} :
                rgba{};
            };

            for (path group: sys::dirs(root.now))
            {
                auto& cells = table.cells;
                int Y = cells.size();
                auto& b = cells[Y][0];
                b.enabled = false;
                b.text.html = bold(blue(str(group.stem()).from(3)));
                b.text.alignment = xy(pix::left, pix::center);
                b.on_change_state = [&](){ on_change_state(b); };
                b.on_change_state();

                for (path theme: sys::dirs(group))
                {
                    int y = Y + 1;
                    int x = cells[y].size();
                    auto& b = cells[y][x];
                    b.enabled = false;
                    b.text.html = bold(black(str(theme.stem()).from(3)));
                    b.text.alignment = xy(pix::left, pix::center);
                    b.on_change_state = [&](){ on_change_state(b); };
                    b.on_change_state();

                    for (path topic: sys::files(extra ? theme/"90 ''Extra''" : theme))
                    {
                        y++;
                        for (int i=0; i<x; i++)
                        {
                            auto& b = cells[y][i];
                            b.enabled = b.text.text != "",
                            b.on_change_state = [&](){ on_change_state(b); },
                            b.on_change_state();
                        }
                        auto& b = cells[y][x];
                        str s = str(topic.stem()).from(3); s.strip("'");
                        b.text.html = extra ? extracolor(s) : black(s);
                        b.text.alignment = xy(pix::left, pix::center);
                        b.on_change_state = [&](){ on_change_state(b); };
                        b.on_change_state();
                        b.name = str(topic);
                    }
                }
            }
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height*13/10;
                selector.coord = xywh(0, 0, W, h);
                table   .coord = xyxy(0, h, W, H);
                replane();
            }

            if (what == &selector)
            {
                int n = selector.object.selected.now;
                path dir = std::filesystem::current_path() / "content";
                extra = n % 2 == 1;
                int m = n / 2;
                root =
                m == 0 ? dir / "10 Elementary":
                m == 1 ? dir / "20 Intermediate 1":
                m == 2 ? dir / "30 Intermediate 2":
                m == 3 ? dir / "80 Advanced":
                "";
                fill();
                replane();
            }

            if (what == &root)
            {
                watcher.dir = root.now;
            }

            if (what == &watcher)
            {
                fill();
                replane();
            }

            if (what == &table)
            {
                link = "one://" + table.cells.notifier->
                notifier->name.now + "|0";
                notify();
            }
        }
    };
}

