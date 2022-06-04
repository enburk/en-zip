#pragma once
#include "app.h"
namespace app::dic
{
    struct html_view : gui::text::page
    {
        int clicked = 0;

        array<str> forbidden_links;

        void on_change (void* what) override
        {
            gui::text::page::on_change(what);

            if (what == &skin)
            {
                padding = xyxy(
                gui::metrics::line::width*3, 0,
                gui::metrics::line::width*3, 0);
            }
        }

        void prelink (xy p)
        {
            p -= view.coord.now.origin;
            p -= view.cell.coord.now.origin;
            auto& block = view.model->block;
            auto token = block.hovered_token(p);

            if (not token
            or token->link != ""
            or token->info != "")
                return;

            auto [l, o] = view.pointed(p);
            if (l < 0 or l >= block.lines.size())
                return;

            auto& line = block.lines[l];
            array<doc::text::token> tokens;
            tokens.reserve(line.tokens.size());
            for (auto& t: line.tokens)
                tokens += {t.text};

            eng::parser::proceed(vocabulary, tokens, forbidden_links);

            for (int i=0; i<line.tokens.size(); i++)
            {
                str s = tokens[i].info;
                if (s.size() < 2) // ignore dots, commas etc.
                line.tokens[i].info = "."; else
                line.tokens[i].link = s;
            }

            int i = 0;
            for (auto& row: line.rows)
            for (auto& solid: row.solids)
            for (auto& token: solid.tokens)
            {
                str s = tokens[i++].info;
                if (s.size() < 2) // ignore dots, commas etc.
                token.info = "."; else
                token.link = s;
            }
        }

        void on_mouse_hover (xy p) override
        {
            if (not touch
            and not sys::keyboard::ctrl)
                prelink(p);

            gui::text::page::on_mouse_hover(p);
        }

        void on_mouse_click (xy p, str button, bool down) override
        {
            gui::text::page::on_mouse_click(p, button, down);

            if (down
            and not sys::keyboard::ctrl
            and link != "")
            {
                if (auto index =
                vocabulary.index(link); index)
                {
                    clicked = *index;
                    notify();
                }
            }
        }
    };
}