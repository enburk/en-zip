#pragma once
#include "app.h"
namespace app::text
{
    void prelink (gui::text::view& view, xy p, array<str> const& forbidden_links)
    {
        auto token = view.hovered_token(p);

        if (not token
        or token->link != ""
        or token->info != "")
            return;

        auto& block = view.model.now->block;
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

    struct page : gui::text::page
    {
        int clicked = 0;

        array<str> forbidden_links;

        page () { tooltip.enabled = false; }

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

        void on_mouse_hover (xy p) override
        {
            if (not touch
            and not sys::keyboard::ctrl)
                prelink(view, p, forbidden_links);

            gui::text::page::on_mouse_hover(p);
        }

        void on_mouse_click (xy p, str button, bool down) override
        {
            gui::text::page::on_mouse_click(p, button, down);

            if (down
            and not sys::keyboard::ctrl
            and link != "")
            {
                if (auto index = vocabulary.index(link))
                {
                    clicked = *index;
                    notify();
                }
            }
        }
    };

    struct view : page
    {
        view ()
        {
            scroll.x.mode = gui::scroll::mode::none;
            scroll.y.mode = gui::scroll::mode::none;
        }
    };

    struct cell : gui::text::view
    {
        str link;
        int clicked = 0;
        array<str> forbidden_links;

        bool mouse_sensible (xy p) override { return true; }

        void on_mouse_hover (xy p) override
        {
            prelink(*this, p, forbidden_links);

            auto token = hovered_token(p);
            link = token ? token->link : "";
            mouse_image = link == "" ? "arrow" : "hand";

            bool same = true;
            for (auto token: visible_tokens())
            {
                auto style_index = token->style;
                if (link != "" and token->link == link)
                {
                    auto style = style_index.style();
                    style.color = gui::skins[skin].link.first;
                    style_index = pix::text::style_index(style);
                }
                for (auto& glyph: token->glyphs)
                if (glyph.style_index != style_index) {
                    glyph.style_index  = style_index;
                    same = false; }
            }
            if (not same) update();

        }

        void on_mouse_click (xy p, str button, bool down) override
        {
            auto token = hovered_token(p);

            if (down
            and token
            and button == "left"
            and not sys::keyboard::ctrl
            and token->link != "")
            {
                if (auto index = vocabulary.index(token->link))
                {
                    clicked = *index;
                    notify();
                }
            }
        }

        void on_mouse_leave () override
        {
            link = "";
            bool same = true;
            for (auto token: visible_tokens())
            for (auto& glyph: token->glyphs)
            if (glyph.style_index != token->style) {
                glyph.style_index  = token->style;
                same = false; }
            if (not same) update();
        }
    };
}