#pragma once
#include "studia_dic_area.h"
#include "studio_build_dic.h"
namespace studio::dic
{
    struct studio:
    widget<studio>
    {
        area area;
        app::dic::app app; // after area
        gui::splitter splitter;

        void reload () { app.reload(); area.reload(); }

        void on_change (void* what) override
        {
            if (what == &splitter
            or  what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int d = gui::metrics::line::width*10;
                int x = splitter.set("studio::dic::splitter", 25, 55, 75);

                area.coord = xyxy(0, 0, x, H);
                app .coord = xyxy(x, 0, W, H);
            }

            if (what == &area)
            {
                focus = &app.list;

                app.list.select(area.clicked);

                if (area.search.alpha.to > 0)
                {
                    str s = area.search.object.request.now;
                    if (s == "") return;

                    array<gui::text::range> highlights;
                    auto& view = app.left.card.object.text;
                    auto& text = view.model.now->block;
                    for (auto& line:  text.lines)
                    for (auto& token: line.tokens)
                    if (token.text.contains(s))
                    highlights += token.range;
                    view.highlights =
                        highlights;
                }
            }

            if (what == &focus_on
                and focus_on.now
                and not focus)
                focus = &app;
        }

        void on_key(str key, bool down, bool input) override
        {
            if (key == "tab" and down)
            {
                if (focus.now == &area)
                    focus = &app; else
                    focus = &area;
            }

            if (focus.now)
                focus.now->on_key(
                key, down, input);
        }
    };
}
