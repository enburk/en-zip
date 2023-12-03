#pragma once
#include "studio_build_one+.h"
namespace studia::one
{
namespace report = studio::one::report;

    struct resorts:
    widget<resorts>
    {
        struct consobar:
        widget<consobar>
        {
            str link;
            gui::console audiom, audiop, audioq;
            gui::console videom, videop, videoq;
            array<gui::console*> consoles;
            array<report::report*> reports;
            array<int> readiness;
            consobar ()
            {
                consoles += &audiom; reports += &report::audiom; readiness += 1;
                consoles += &videom; reports += &report::videom; readiness += 1;
                consoles += &audiop; reports += &report::audiop; readiness += 1;
                consoles += &videop; reports += &report::videop; readiness += 1;
                consoles += &audioq; reports += &report::audioq; readiness += 1;
                consoles += &videoq; reports += &report::videoq; readiness += 1;
                for (auto& c: consoles)
                c->hide();
            }
            void on_change (void* what) override
            {
                if (what == &coord)
                for (auto& c: consoles)
                c->coord = coord.now.local();

                for (auto& c: consoles)
                if (what == &c->link)
                link = c->link,
                notify();
            }
            void reload ()
            {
                for (auto& c: consoles) c->clear();
                for (auto& r: readiness) r = 0;
            }
            void prepare (int n)
            {
                if (readiness[n] == 0) {
                    readiness[n] = 1;
                    reports[n]->load();
                   *consoles[n] <<
                    reports[n]->log;
                }
            }
        };

        str link;
        gui::area<gui::selector> selector;
        gui::area<consobar> consobar;

        resorts ()
        {
            int i = 0;
            auto& sel = selector.object;
            sel.buttons(i++).text.text = "audio-";
            sel.buttons(i++).text.text = "video-";
            sel.buttons(i++).text.text = "audio+";
            sel.buttons(i++).text.text = "video+";
            sel.buttons(i++).text.text = "audio?";
            sel.buttons(i++).text.text = "video?";
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
                consobar.object.prepare(n);
            }
            if (what == &alpha && shown())
            {
                int n =
                selector.object.selected.now;
                consobar.object.prepare(n);
            }
            if (what == &consobar)
            {
                link = consobar.object.link;
                notify();
            }
        }
    };
}

