#pragma once
#include "app.h"
namespace app
{
    struct Contents:
    widget<Contents>
    {
        gui::canvas canvas;
        gui::table table;
        gui::selector selector;
        binary_property<str> selected;
        content::unit* root = nullptr;
        bool extraful = false;

        void fill()
        {
            if (not root) return;
            if (root->units.empty()) return;
            int n = selector.selected.now;
            if (n < 0) return;

            table.cells.clear();
            bool extra = (n % 2 == 1) and extraful;
            if (extraful) n = n / 2;
            content::unit& cycle =
                root->units[n];

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

            const bool GH = true; // groups horizontally
            const bool GV = not GH;

            for (auto& group: cycle.units)
            {
                if (group.kind !=
                    content::unit::theme)
                    continue;

                auto& cells = table.cells;
                int X = GV or cells.empty() ? 0 : cells[0].size();
                int Y = GH ? 0 : cells.size();
                auto& b = table.cell(X,Y);
                b.enabled = false;
                b.text.html = bold(black(group.name));
                b.text.alignment = xy(pix::left, pix::center);
                b.on_change_state = [&](){ on_change_state(b); };
                b.on_change_state();

                for (auto theme: group.units)
                {
                    if (theme.kind !=
                        content::unit::theme)
                        continue;

                    if (GH) Y += 2;
                    int y = GH ? Y : Y + 1;
                    int x = GH ? X : cells[y].size();
                    auto& b = table.cell(x,y);
                    b.enabled = false;
                    b.text.html = bold(blue(theme.name));
                    b.text.alignment = xy(pix::left, pix::center);
                    b.on_change_state = [&](){ on_change_state(b); };
                    b.on_change_state();

                    content::unit& Theme =
                        extra and not
                        theme.units.empty() ?
                        theme.units.back() :
                        theme;

                    for (auto topic: Theme.units)
                    {
                        if (topic.kind !=
                            content::unit::topic)
                            continue;

                        str name = topic.name;
                        if (name.starts_with("''")
                        and name.  ends_with("''")) {
                            name.truncate(); name.erase(0);
                            name.truncate(); name.erase(0);
                            name = extracolor(
                            name); }

                        y++;
                        if (GH) Y++;
                        auto& b = table.cell(x,y);
                        b.text.html = name;
                        b.text.alignment = xy(pix::left, pix::center);
                        b.on_change_state = [&](){ on_change_state(b); };
                        b.on_change_state();
                        b.name = topic.path;
                    }
                }
            }

            table.refresh();
        }

        void reload (str name, content::unit& root_)
        {
            root = &root_;
            extraful = name ==
                "app::Ones";

            int i = 0;
            selector.selected = -1;
            selector.buttons.clear();
            for (auto cycle: root->units)
            {
                selector.buttons(i++).text.text =
                    cycle.name;

                if (extraful)
                selector.buttons(i++).text.html =
                    extracolor("Extra");
            }
            selector.selected = 0;
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                canvas.coord =
                coord.now.local();
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height*13/10;
                selector.coord = xywh(0, 0, W, h);
                table   .coord = xyxy(0, h, W, H);
                table.refresh();
            }

            if (what == &skin)
            {
                canvas.color =
                gui::skins[skin].
                ultralight.first;
            }

            if (what == &selector)
            {
                fill();
            }

            if (what == &table)
            {
                selected.was = selected.now;
                selected.now = table.cells.notifier->
                notifier->name.now;
                notify();
            }
        }
    };
}