#pragma once
#include "studio_build_dic+.h"
namespace studia::dic
{
    namespace report =
    studio::dic::report;

    struct reports:
    widget<reports>
    {
        struct consobar:
        widget<consobar>
        {
            str link;
            gui::console errors;
            gui::console usages;
            gui::console statts;
            gui::console timing;
            array<gui::console*> consoles;
            consobar ()
            {
                app::logs::times = timing;
                timing << app::dicdata.report;
                timing << app::dicdata.error;
                timing << "";
                timing << app::appdata.report;
                timing << app::appdata.error;
                timing << "";

                consoles += &errors;
                consoles += &usages;
                consoles += &statts;
                consoles += &timing;
                for (auto& c: consoles)
                c->hide();
            }
            void on_change (void* what) override
            {
                if (what == &coord)
                for (auto& c: consoles)
                c->coord = coord.now.local();

                if (what == &errors.link) { link = errors.link; notify(); }
                if (what == &usages.link) { link = usages.link; notify(); }
                if (what == &statts.link) { link = statts.link; notify(); }
            }
            void reload ()
            {
                report::load();
                errors.clear(); errors << report::errors;
                usages.clear(); usages << report::usages;
                statts.clear(); statts << report::statts;
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
            sel.buttons(i++).text.text = "usages";
            sel.buttons(i++).text.text = "statistics";
            sel.buttons(i++).text.text = "timing";
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

