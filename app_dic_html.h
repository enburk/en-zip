#pragma once
#include "app.h"
namespace app::dic
{
    struct html_view : gui::text::page
    {
        int clicked = 0;

        str highlighted_link;

        array<str> forbidden_links;

        int link (gui::text::token& token, gui::text::line& line)
        {
            if (token.info.size() == 1)
            // ignore dots, commas etc.
                return -1;

            if (token.info != "") {
                auto index = vocabulary.index(token.info);
                return index ? *index : -1; }

            array<doc::text::token> tokens;
            tokens.reserve(line.size());

            for (auto& t: line) {
                tokens += {
                    t.text == (char*)(u8"’") ? "'" : t.text,
                    t.info != "" ? "Text" : ""
                };
            }

            eng::parser::proceed(vocabulary, tokens, forbidden_links);

            for (int i=0; i<line.size(); i++) {
                str s = tokens[i].info;
                if (s == "") s = ".";
                line(i).info = s;
            }

            return link(token, line);
        }

        int link (XY p)
        {
            auto & column = view.lines;

            if (!column.coord.now.includes(p)) return -2;
            auto cp = p - column.coord.now.origin;
            for (auto & line : column)
            {
                if (!line.coord.now.includes(cp)) continue;
                auto lp = cp - line.coord.now.origin;
                for (auto & token : line)
                {
                    if (!token.coord.now.includes(lp)) continue;
                    return link(token, line);
                }
            }
            return -2;
        }

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (down and not sys::keyboard::ctrl)
            {
                if (auto n = link(p); n >= 0 and
                    not forbidden_links.contains(
                        vocabulary[n].title))
                {
                    clicked = n;
                    notify();
                    return;
                }
            }
            gui::text::page::on_mouse_press(p, button, down);
        }

        void on_mouse_hover (XY p) override
        {
            highlighted_link = "";

            if (not touch and not sys::keyboard::ctrl)
            {
                int n = link(p);
                str link = n >= 0 ?
                    vocabulary[n].title : "";
                
                mouse_image = n < 0 or
                    forbidden_links.contains(link) ?
                        "arrow": "hand";


                for (auto & line : view.lines)
                {
                    for (auto & token : line)
                    {
                        auto style_index = token.style;
                        if ((link != "" and token.text == link) or
                            (link != "" and token.info == link))
                        {
                            auto style = style_index.style();
                            style.color = RGBA(0,0,255);
                            style_index = pix::text::style_index(style);
                            highlighted_link = link;
                        }
                        for (auto& g: token.glyphs)
                            g.style_index = style_index;
                        token.update();
                    }
                }
                return;
            }
            else if (sys::keyboard::ctrl)
            {
                for (auto & line : view.lines)
                {
                    for (auto & token : line)
                    {
                        for (auto& g: token.glyphs)
                            g.style_index = token.style;
                        token.update();
                    }
                }
            }
            gui::text::page::on_mouse_hover(p);
        }

        void on_mouse_leave () override
        {
            highlighted_link = "";

            if (!touch)
            {
                for (auto & line : view.lines)
                {
                    for (auto & token : line)
                    {
                        for (auto& g: token.glyphs)
                            g.style_index = token.style;
                        token.update();
                    }
                }
            }
            gui::text::page::on_mouse_leave();
        }
    };
}