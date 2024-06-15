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
        str link;

        contents ()
        {
            int i = 0;
            auto& select = selector.object;
            select.buttons(i++).text.text = "Primary";
            select.buttons(i++).text.html = extracolor("Extra");
            select.buttons(i++).text.text = "Secondary";
            select.buttons(i++).text.html = extracolor("Extra");
            select.buttons(i++).text.text = "Tertiary";
            select.buttons(i++).text.html = extracolor("Extra");
            select.buttons(i++).text.text = "Quaternaty";
            select.buttons(i++).text.html = extracolor("Extra");
            select.buttons(i++).text.text = "Quinary";
            select.buttons(i++).text.html = extracolor("Extra");
            select.buttons(i++).text.text = "Senary";
            select.buttons(i++).text.html = extracolor("Extra");
            select.selected = 0;
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

            bool extra = selector.object.selected.now % 2 == 1;

            const bool GH = true; // groups horizontally
            const bool GV = not GH;

            for (path group: sys::dirs(root.now))
            {
                auto& cells = table.cells;
                int X = GV or cells.empty() ? 0 : cells[0].size();
                int Y = GH ? 0 : cells.size();
                auto& b = table.cell(X,Y);
                b.enabled = false;
                b.text.html = bold(black(str(group.stem()).from(3)));
                b.text.alignment = xy(pix::left, pix::center);
                b.on_change_state = [&](){ on_change_state(b); };
                b.on_change_state();

                for (path theme: sys::dirs(group))
                {
                    if (GH) Y += 2;
                    int y = GH ? Y : Y + 1;
                    int x = GH ? X : cells[y].size();
                    auto& b = table.cell(x,y);
                    b.enabled = false;
                    b.text.html = bold(blue(str(theme.stem()).from(3)));
                    b.text.alignment = xy(pix::left, pix::center);
                    b.on_change_state = [&](){ on_change_state(b); };
                    b.on_change_state();

                    for (path topic: sys::files(extra ? theme/"90 ''Extra''" : theme))
                    {
                        y++;
                        if (GH) Y++;
                        auto& b = table.cell(x,y);
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
                table.refresh();
            }

            if (what == &selector)
            {
                int n = selector.object.selected.now;
                path dir = std::filesystem::current_path() / "content";
                int m = n / 2;
                root =
                m == 0 ? dir / "10 Primary":
                m == 1 ? dir / "20 Secondary":
                m == 2 ? dir / "30 Tertiary":
                m == 3 ? dir / "40 Quaternary":
                m == 4 ? dir / "50 Quinary":
                m == 5 ? dir / "60 Senary":
                "";
                fill();
                table.refresh();
            }

            if (what == &root)
            {
                watcher.dir = root.now;
            }

            if (what == &watcher)
            {
                fill();
                table.refresh();
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

