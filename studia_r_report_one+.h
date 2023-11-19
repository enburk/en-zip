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
            consobar ()
            {
                consoles += &audiom;
                consoles += &videom;
                consoles += &audiop;
                consoles += &videop;
                consoles += &audioq;
                consoles += &videoq;
                for (auto& c: consoles)
                c->hide();
            }
            void on_change (void* what) override
            {
                if (what == &coord)
                for (auto& c: consoles)
                c->coord = coord.now.local();

                if (what == &audiom.link) { link = audiom.link; notify(); }
                if (what == &videom.link) { link = videom.link; notify(); }
                if (what == &audiop.link) { link = audiop.link; notify(); }
                if (what == &videop.link) { link = videop.link; notify(); }
                if (what == &audioq.link) { link = audioq.link; notify(); }
                if (what == &videoq.link) { link = videoq.link; notify(); }
            }
            void reload ()
            {
                report::load();
                audiom.clear(); audiom << report::audiom;
                videom.clear(); videom << report::videom;
                audiop.clear(); audiop << report::audiop;
                videop.clear(); videop << report::videop;
                audioq.clear(); audioq << report::audioq;
                videoq.clear(); videoq << report::videoq;
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
            }
            if (what == &consobar)
            {
                link = consobar.object.link;
                notify();
            }
        }
    };
}

