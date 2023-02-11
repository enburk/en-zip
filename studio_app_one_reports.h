#pragma once
#include "app_one.h"
namespace studio::one
{
    namespace report
    {
        optional_log anomal1;
        optional_log anomal2;
        optional_log audiom, audiop, audioq;
        optional_log videom, videop, videoq;
    }
    struct reports:
    widget<reports>
    {
        struct consobar:
        widget<consobar>
        {
            gui::console anomal1;
            gui::console anomal2;
            gui::console audiom, audiop, audioq;
            gui::console videom, videop, videoq;
            array<gui::console*> consoles;
            consobar ()
            {
                report::anomal1 = anomal1;
                report::anomal2 = anomal2;
                report::audiom = audiom;
                report::videom = videom;
                report::audiop = audiop;
                report::videop = videop;
                report::audioq = audioq;
                report::videoq = videoq;
                consoles += &anomal1;
                consoles += &anomal2;
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
            }
        };

        gui::area<gui::selector> selector;
        gui::area<consobar> consobar;

        reports ()
        {
            int i = 0;
            selector.object.buttons(i++).text.text = "Br/Am";
            selector.object.buttons(i++).text.text = "{}()[]";
            selector.object.buttons(i++).text.text = "audio-";
            selector.object.buttons(i++).text.text = "video-";
            selector.object.buttons(i++).text.text = "audio+";
            selector.object.buttons(i++).text.text = "video+";
            selector.object.buttons(i++).text.text = "audio?";
            selector.object.buttons(i++).text.text = "video?";
            selector.object.maxwidth = max<int>();
            selector.object.selected = 0;
        }

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
        }
    };
}

