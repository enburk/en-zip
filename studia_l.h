#pragma once
#include "studia_l_app.h"
#include "studia_l_search.h"

struct studia_l:
widget<studia_l>
{
    studia::content one;
    studia::content two;
    array<gui::base::widget*> planes;
    gui::area<gui::selector> selector;

    studia_l ()
    {
        path dir = std::filesystem::current_path();
        one.name = "studia::one";
        two.name = "studia::two";
        one.root = dir/"content";
        two.root = dir/"catalog";

        planes += &one;
        planes += &two;
        //planes += &search;

        for (auto p:
            planes.from(1))
            p->hide();

        int i = 0;
        auto& select = selector.object;
        select.buttons(i++).text.text = "course";
        select.buttons(i++).text.text = "catalogs";
        select.buttons(i++).text.text = "search";
        select.selected = 0;
    }

    void reload ()
    {
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

            for (auto p: planes)
                p->coord = xyxy(
                    0, h, W, H);
        }

        if (what == &selector)
        {
            int n = selector.object.selected.now;
            for (int i=0; i<planes.size(); i++)
            planes[i]->show(i == n);
        }

        for (auto p: planes)
        if (what == &p->link)
            link = p->link,
            notify(&link);
    }
};

