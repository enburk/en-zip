#pragma once
#include "studio_app_one_content.h"
#include "studio_app_one_editor.h"
#include "studio_app_one_random.h"
namespace studio::one
{
    struct area:
    widget<area>
    {
        array<gui::base::widget*> areas;
        gui::area<gui::canvas> toolbar;
        gui::area<gui::canvas> areabar;
        gui::area<content> content;
        gui::area<editor> editor;
        gui::area<random> random;
        gui::radio::group select;
        gui::splitter splitter;

        area ()
        {
            toolbar.object.color = gui::skins[skin].light.first;

            areas += &editor;
            areas += &random;

            for (int i=1; i<
            areas.size(); i++)
            areas[i]->hide();

            int i = 0;
            select(i++).text.text = "editor";
            select(i++).text.text = "random";
            select(0).on = true;
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
                int d = gui::metrics::line::width*10;

                splitter.lower = W *  5'00 / 100'00;
                splitter.upper = W * 15'00 / 100'00;
                str s = "studio::dic::area::splitter.permyriad";
                int p = sys::settings::load(s, 10'00);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower, splitter.upper);
                splitter.coord = xyxy(x-d, 0, x+d, H);

                toolbar.coord = xywh(0, 0, W, h);
                areabar.coord = xyxy(x, h, W, H);
                content.coord = xyxy(0, h, x, H);

                select.coord = toolbar.object.coord.now;
                select(0).coord = xywh(w*0, 0, w, h);
                select(1).coord = xywh(w*1, 0, w, h);

                for (auto x: areas) x->coord =
                areabar.object.coord.now +
                areabar.coord.now.origin;
            }

            if (what == &splitter) {
                sys::settings::save(
                "studio::one::area::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }

            if (what == &select)
            {
                int n = select.notifier_index;
                for (int i=0; i<
                areas.size(); i++)
                areas[i]->show(i == n);
            }
        }
    };
}
