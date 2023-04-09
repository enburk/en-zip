#pragma once
#include "studia_one_area.h"
#include "studia_one_preview.h"
#include "studia_one_reports.h"
namespace studio::one
{
    struct studio:
    widget<studio>
    {
        area left;
        app::dic::app dic;
        app::one::app app;
        gui::area<preview> preview;
        gui::area<reports> reports;
        array<gui::base::widget*> right;
        gui::area<gui::selector> selector;
        gui::splitter splitter;

        studio ()
        {
            right += &dic;
            right += &preview;
            right += &reports;
            right += &app;

            for (int i=1; i<
            right.size(); i++)
            right[i]->hide();

            int i = 0;
            auto& select = selector.object;
            select.buttons(i++).text.text = "dictionary";
            select.buttons(i++).text.text = "preview";
            select.buttons(i++).text.text = "reports";
            select.buttons(i++).text.text = "app";
            select.maxwidth = max<int>();
            select.selected = 0;
        }

        void reload ()
        {
            dic.reload();
            app.reload();
            reports.object.reload();
        }

        void on_change (void* what) override
        {
            if (what == &splitter
            or  what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*13/10;
                int l = gui::metrics::line::width;
                int x = splitter.set("studio::one::splitter", 25, 40, 75);

                selector.coord = xywh(0, 0, x, h);
                left    .coord = xyxy(0, h, x, H);
                dic     .coord = xyxy(x, 0, W, H);
                app     .coord = xyxy(x, 0, W, H);
                preview .coord = xyxy(x, 0, W, H);
                reports .coord = xyxy(x, 0, W, H);
            }

            if (what == &selector)
            {
                int n = selector.object.selected.now;
                for (int i=0; i<right.size(); i++)
                right[i]->show(i == n);
            }

            if (what == &reports)
            {
                str path, line;
                reports.object.link.split_by("|", path, line);
                left.contents.object.selected = path.c_str(); 
                left.editor.object.editor.go(doc::place{
                std::stoi(line), 0});
                focus = &left;
            }
        }

        void on_key(str key, bool down, bool input) override
        {
            if (key == "tab" and down)
            {
                if (focus.now != &left)
                    focus = &left; else
                    for (auto& r: right)
                    if (r->alpha.now == 255)
                    focus = r;
            }

            if (focus.now)
                focus.now->on_key(
                key, down, input);
        }
    };
}

