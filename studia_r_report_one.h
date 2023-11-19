#pragma once
#include "studio_build_one+.h"
namespace studia::one
{
namespace report = studio::one::report;

    struct reports:
    widget<reports>
    {
        struct consobar:
        widget<consobar>
        {
            str link;
            gui::console errors;
            gui::console anomal;
            gui::console duples;
            array<gui::console*> consoles;
            consobar ()
            {
                consoles += &errors;
                consoles += &anomal;
                consoles += &duples;
                for (auto& c: consoles)
                c->hide();
            }
            void on_change (void* what) override
            {
                if (what == &coord)
                for (auto& c: consoles)
                c->coord = coord.now.local();

                if (what == &errors.link) { link = errors.link; notify(); }
                if (what == &anomal.link) { link = anomal.link; notify(); }
                if (what == &duples.link) { link = duples.link; notify(); }
            }
            void reload ()
            {
                report::load();
                errors.clear(); errors << report::errors;
                anomal.clear(); anomal << report::anomal;
                duples.clear(); duples << report::duples;
            }
        };

        str link;
        gui::area<gui::selector> selector;
        gui::area<consobar> consobar;

        reports ()
        {
            int i = 0;
            auto& sel = selector.object;
            sel.buttons(i++).text.text = "errors";
            sel.buttons(i++).text.text = "anomal";
            sel.buttons(i++).text.text = "duplicates";
            sel.selected = 0;
            reload();
        }

        void reload () { consobar.object.reload(); }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*13/10;
                int l = gui::metrics::line::width;
                selector.coord = xywh(0, 0, W, h);
                consobar.coord = xyxy(0, h, W, H);
            }
            if (what == &selector)
            {
                auto& consoles = 
                consobar.object.consoles;
                int n = selector.object.selected.now;
                for (int i=0; i<consoles.size(); i++)
                consoles[i]->show(i == n);
            }
            if (what == &consobar)
            {
                link = consobar.object.link;
                notify();
            }
        }
    };
}

