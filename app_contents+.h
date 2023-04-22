#pragma once
#include "app.h"
namespace app
{
    struct Contents:
    widget<Contents>
    {
        gui::canvas canvas;

        struct flist:
        widget<flist>
        {
            str selected;
            array<str> paths;
            gui::widgetarium<gui::button> list;
            //void on_change (void* w) override {
            //    if (w == &list) {
            //    selected = paths[
            //    list.notifier_index];
            //    notify(); } }
        };

        flist flist;
        gui::scroller<
        gui::vertical>
             scroller;

        gui::binary_property<str> selected;

        struct block
        {
            array<str> path;
            array<str> topics;
        };
        array<block> blocks;

        void refresh ()
        {
        }

        void replane ()
        {
            int t = scroller.top;
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*12/10;
            int l = gui::metrics::line::width;
            int n = W/(9*h);
            int w = W/n;
            int x = 0;
            int y = 0;
            int r = 0;

            auto title = [](str s)
            {
                if (s.starts_with("''")
                and s.  ends_with("''")) {
                    s.truncate(); s.erase(0);
                    s.truncate(); s.erase(0);
                    s = lightblue(s); }
                return s;
            };

            flist.list.clear();

            for (auto& block: blocks)
            {
                int yy = y;
                str spaces = " ";
                
                for (str s: block.path)
                {
                    // flist.paths += 
                    auto& line = flist.list.emplace_back();
                    line.coord = xywh(x, yy, w, h);
                    line.text.alignment = xy(pix::left, pix::center);
                    line.text.html = spaces + yellow(bold(title(s)));
                    yy += h; spaces += " ";
                }

                for (str s: block.topics)
                {
                    auto& line = flist.list.emplace_back();
                    line.coord = xywh(x, yy, w, h);
                    line.text.alignment = xy(pix::left, pix::center);
                    line.text.html = spaces + title(s);
                    yy += h;
                }

                r = max(r, yy);

                x += w;
                if (x + w > W)
                {
                    x = 0;
                    y += r + h;
                    r = 0;
                }
            }

            if (x > 0)
                y += r;

            flist     .coord = xyxy(0, 0, W, H);
            scroller  .coord = xyxy(W, 0, W, H);
            flist.list.coord = xywh(0, 0, W, y);

            scroller.span = y;
            scroller.step = h;
            scroller.top  = t; xywh rr =
            flist.list.coord;  rr.y = -scroller.top;
            flist.list.coord = rr;
        }

        bool topic (content::unit& unit)
        {
            bool ok = true;
            for (auto& u: unit.units)
            if (u.name != "") {
            ok = false; break; }
            return ok;
        }

        array<block> fill (content::unit& parent)
        {
            array<block> blocks; block block;

            for (auto& unit: parent.units)
                if (topic(unit))
                block.topics +=
                unit.name;

            if (not block.topics.empty()) {
                block.path = parent.path.split_by("/");
                blocks += block; }

            for (auto& unit: parent.units)
                if (not topic(unit))
                blocks += fill(unit);

            return blocks;
        }

        void reload (str name, content::unit& root)
        {
            blocks = fill(root);
            replane();
            refresh();
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                replane();
                refresh();
            }

            if (what == &skin)
            {
                canvas.color =
                gui::skins[skin].
                ultralight.first;
            }

            if (what == &flist)
            {
                selected.was = selected.now;
                selected.now = flist.selected;
                notify();
            }

            if (what == &selected)
            {
                refresh();
            }

            if (what == &scroller) { xywh r =
                flist.list.coord; r.y = -scroller.top;
                flist.list.coord = r;
            }
        }

        bool on_mouse_wheel (xy p, int delta) override
        {
            int h = scroller.step;
            delta /= abs(delta) < 20 ? abs(delta) : 20;
            delta *= h > 0 ? h : gui::metrics::text::height;
            if (sys::keyboard::ctrl ) delta *= 5;
            if (sys::keyboard::shift) delta *= coord.now.h;
            int d = flist.coord.now.h - flist.list.coord.now.h; // may be negative
            int y = flist.list.coord.now.y + delta;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroller.top =-y;
            return true;
        }
    };
}