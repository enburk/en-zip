#pragma once
#include "app_one.h"
namespace studia
{
    struct extras:
    widget<extras>
    {
        binary_property<path> root;
        sys::directory_watcher watcher;
        std::atomic<bool> reload = true;
        property<gui::time> timer;
        str link;

        gui::widgetarium<
        gui::widgetarium<
        gui::button>> flist;

        gui::scroller<
        gui::vertical>
             scroller;

        void replane ()
        {
            int t = scroller.top;
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*12/10;
            int Y = flist.size();
            int X = 1;
            int w = 0;

            for (auto& row: flist)
            X = max(X, row.size());
            w = W/X;

            for (int y=0; y<Y; y++)
            for (int x=0; x<X; x++)
            if (y < flist   .size())
            if (x < flist[y].size())
            flist[y][x].coord = xywh(x*w, 0, w, h),
            flist[y]   .coord = xywh(0, y*h, W, h);

            flist      .coord = xyxy(0, 0, W, Y*h);
            scroller   .coord = xyxy(W, 0, W, H);

            scroller.step = h;
            scroller.span = Y*h;
            scroller.top  = t; xywh rr =
            flist.coord;  rr.y = -scroller.top;
            flist.coord = rr;
        }

        void fill ()
        {
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
                int Y = flist.size();
                auto& b = flist[Y][0];
                b.enabled = false;
                b.text.html = bold(blue(str(group.stem()).from(3)));
                b.text.alignment = xy(pix::left, pix::center);
                b.on_change_state = [&](){ on_change_state(b); };
                b.on_change_state();

                for (path theme: sys::dirs(group))
                {
                    int y = Y + 1;
                    int x = flist[y].size();
                    auto& b = flist[y][x];
                    b.enabled = false;
                    b.text.html = black(str(theme.stem()).from(3));
                    b.text.alignment = xy(pix::left, pix::center);
                    b.on_change_state = [&](){ on_change_state(b); };
                    b.on_change_state();

                    for (path topic: sys::files(theme/"90 ''Extra''"))
                    {
                        y++;
                        for (int i=0; i<x; i++)
                        {
                            auto& b = flist[y][i];
                            b.enabled = b.text.text != "",
                            b.on_change_state = [&](){ on_change_state(b); },
                            b.on_change_state();
                        }
                        auto& b = flist[y][x];
                        str s = str(topic.stem()).from(3); s.strip("'");
                        b.text.html = extracolor(s);
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
                replane();
            }

            if (what == &root)
            {
                watcher.cancel();
                watcher.dir = root.now;
                watcher.action = [this]
                (path path, str what) {
                reload = true; };
                watcher.watch();
                reload = true;
                what = &timer;
            }

            if (what == &timer and reload)
            {
                reload = false;
                flist.clear();
                fill();
                replane();
            }

            if (what == &flist)
            {
                link = "one://" + flist.notifier->
                notifier->name.now + "|0";
                notify();
            }

            if (what == &scroller) { xywh r =
                flist.coord; r.y = -scroller.top;
                flist.coord = r;
            }
        }

        bool on_mouse_wheel (xy p, int delta) override
        {
            int h = scroller.step;
            delta /= abs(delta) < 20 ? abs(delta) : 20;
            delta *= h > 0 ? h : gui::metrics::text::height;
            if (sys::keyboard::ctrl ) delta *= 5;
            if (sys::keyboard::shift) delta *= coord.now.h;
            int d = coord.now.h - flist.coord.now.h; // may be negative
            int y = flist.coord.now.y + delta;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroller.top =-y;
            return true;
        }
    };
}

