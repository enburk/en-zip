#pragma once
#include "studia_r_extra.h"
#include "studia_r_report_dic.h"
#include "studia_r_report_one.h"
#include "studia_r_report_one+.h"
#include "studia_r_search.h"

struct studia_r:
widget<studia_r>
{
    app::dic::view dic;
    studia::extras extra;
    studia::dic::reports dics;
    studia::one::reports ones;
    studia::one::resorts oness;
    studia::one::search search;
    array<gui::base::widget*> planes;
    gui::area<gui::selector> selector;
    unary_property<str> link;

    studia_r ()
    {
        path dir = std::filesystem::current_path();
        extra.root = dir/"content/10 Elementary";

        planes += &dic;
        planes += &extra;
        planes += &dics;
        planes += &ones;
        planes += &oness;
        planes += &search;

        for (auto p:
            planes.from(1))
            p->hide();

        int i = 0;
        auto& select = selector.object;
        select.buttons(i++).text.text = "dictionary";
        select.buttons(i++).text.text = "extras";
        select.buttons(i++).text.text = "reports: dic";
        select.buttons(i++).text.text = "reports: one";
        select.buttons(i++).text.text = "resources";
        select.buttons(i++).text.text = "search: one";
        select.selected = 0;
    }

    void reload ()
    {
        dic   .reload();
        dics  .reload();
        ones  .reload();
        oness .reload();
        search.reload();
    }

    void click (str link)
    {
        if (link.starts_with("dic://"))
        {
            link = link.from(6);
            if (link == "")
                return;
            
            focus = &dic.list;

            dic.list.select(std::stoi(link));
        }
        if (link.starts_with("dictionary://"))
        {
            link = link.from(13);
            if (link == "")
                return;
            
            auto index = app::vocabulary.index(link);
            if (!index)
                return;

            focus = &dic.list;

            dic.list.select(*index);
        }
        if (link.starts_with("highlight://"))
        {
            link = link.from(12);
            if (link == "")
                return;

            array<gui::text::range> highlights;
            auto& view = dic.left.card.object.text;
            auto& text = view.model.now->block;
            for (auto& line:  text.lines)
            for (auto& token: line.tokens)
            if (token.text.contains(link))
            highlights += token.range;
            view.highlights =
            highlights;
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

        if (what == &dics)
            link = dics.link,
            notify(&link);

        if (what == &extra)
            link = extra.link,
            notify(&link);

        if (what == &ones)
            link = ones.link,
            notify(&link);
        
        if (what == &oness)
            link = oness.link,
            notify(&link);

        if (what == &search.link)
            link = "one://" +
            search.link,
            notify(&link);
    }
};

