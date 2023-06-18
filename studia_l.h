#pragma once
#include "studia_l_content.h"
#include "studia_l_media.h"
#include "studia_l_search.h"

struct studia_l:
widget<studia_l>
{
    studia::Content one;
    studia::Content two;
    studia::Media media;
    studia::dic::search search;
    array<gui::base::widget*> planes;
    gui::area<gui::selector> selector;
    unary_property<str> link;

    studia_l ()
    {
        path dir = std::filesystem::current_path();
        one.name = "studia::one";
        two.name = "studia::two";
        one.root = dir/"content";
        two.root = dir/"catalog";

        planes += &one;
        planes += &two;
        planes += &media;
        planes += &search;

        for (auto p:
            planes.from(1))
            p->hide();

        int i = 0;
        auto& select = selector.object;
        select.buttons(i++).text.text = "course";
        select.buttons(i++).text.text = "catalog";
        select.buttons(i++).text.text = "media";
        select.buttons(i++).text.text = "search dic";
        select.selected = 0;
    }

    void reload ()
    {
    }

    void click (str link)
    {
        if (link.starts_with("one://"))
        {
            link = link.from(6);
            if (link == "")
                return;
            
            selector.object.
                selected = 0;

            str path, line;
            link.split_by("|", path, line);
            one.contents.object.selected = str2path(path); 
            one.editor.object.editor.go(doc::place{
            std::stoi(line), 0});
        }
        if (link.starts_with("file://"))
        {
            link = link.from(7);
            if (link == "")
                return;

            selector.object.
                selected = 2;

            media.detail.object.
                select(link);
        }
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

        if (what == &media)
            link = "dic://" +
            std::to_string(media.clicked),
            notify(&link);

        if (what == &search.link)
            link = "dictionary://" +
            search.link,
            notify(&link);

        if (what == &search.request)
            link = "highlight://" +
            search.request.now,
            notify(&link);
    }
};

