#pragma once
#include "app.h"
namespace app::dic
{
    struct html_view : gui::text::page
    {
        int clicked = 0;

        array<str> forbidden_links;

        int link (gui::text::token& token, gui::text::line& line)
        {
            if (token.info != "")
            if (auto index = vocabulary.index(token.info); index)
                return token.info.size() > 1 ?
                // ignore dots, commas etc.
                    *index : -1;

            int pointed = -1;
            for (int i=0; i<line.size(); i++)
                if (&line(i) == &token) {
                    pointed = i;
                    break; }

            if (pointed == -1) return *vocabulary.index("nonsense");

            auto lower   = [](str s){ return eng::asciized(s).ascii_lowercased(); };
            auto capital = [](str s){ str c = eng::asciized(s).upto(1);
                return c >= "A" and c <= "Z"; };

            str Longest; int N = 0; int I0 = 0; int I1 = 0;
            str longest; int n = 0; int i0 = 0; int i1 = 0;

            for (int start=0; start<=pointed; start++)
            {
                str Attempt = "";
                str attempt = "";

                for (int i=start; i<=pointed; i++) {
                    attempt += lower(line(i).text);
                    Attempt += line(i).text;
                }

                int i = vocabulary.lower_bound(Attempt);
                str S = vocabulary[i].title;
                str s = lower(S);

                if (not s.starts_with(attempt))
                    continue;

                if (S != s) s = vocabulary.lower_bound(attempt);

                bool Better = Attempt == S and Longest.size() < S.size();
                bool better = attempt == s and longest.size() < s.size();
                Better = Better and start == pointed or not forbidden_links.contains(S);
                better = better and start == pointed or not forbidden_links.contains(s);
                if (Better) { Longest = S; N = i; I0 = start; I1 = pointed; }
                if (better) { longest = s; n = i; i0 = start; i1 = pointed; }

                str Sentinel = Attempt;
                str sentinel = attempt;

                for (int t=pointed+1; t<line.size(); t++)
                {
                    bool valid = false;
                    str Candidate = Attempt + line(t).text;
                    str candidate = attempt + lower(line(t).text);
                    for (auto j=i; j < vocabulary.size(); j++)
                    {
                        S = vocabulary[j].title;
                        s = lower(S);

                        if (not forbidden_links.contains(S)) {
                            bool Better = Candidate == S and Longest.size() < S.size();
                            bool better = candidate == s and longest.size() < s.size();
                            Better = Better and not forbidden_links.contains(S);
                            better = better and not forbidden_links.contains(s);
                            if (Better) { Longest = S; N = j; I1 = t; }
                            if (better) { longest = s; n = j; i1 = t; }
                        }

                        if (s.starts_with(candidate) or
                            candidate.starts_with(s))
                            valid = true;

                        if (not s.starts_with(Sentinel) and
                            not s.starts_with(sentinel))
                            break;
                    }
                    if (not valid) break;
                    Attempt = Candidate;
                    attempt = candidate;
                }
            }

            if (Longest.size() <= longest.size()) {
                Longest = longest; N = n;
                I0 = i0; I1 = i1; }

            if (Longest.size() > 0) // remember it
                for (int i=I0; i<=I1; i++)
                    line(i).info =
                    vocabulary[N].
                    title;

            if (Longest.size() > 1)
            // ignore dots, commas etc.
                return N;

            return -1;
        }

        int link (XY p)
        {
            auto & column = view.column;

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
            if (not touch and not sys::keyboard::ctrl)
            {
                int n = link(p);
                str link = n >= 0 ?
                    vocabulary[n].title : "";
                
                mouse_image = n < 0 or
                    forbidden_links.contains(link) ?
                        "arrow": "hand";


                for (auto & line : view.column)
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
                        }
                        for (auto & glyph : token)
                        {
                            auto g = glyph.value.now;
                            g.style_index = style_index;
                            glyph.value = g;
                        }
                    }
                }
                return;
            }
            else if (sys::keyboard::ctrl)
            {
                for (auto & line : view.column)
                {
                    for (auto & token : line)
                    {
                        for (auto & glyph : token)
                        {
                            auto g = glyph.value.now;
                            g.style_index = token.style;
                            glyph.value = g;
                        }
                    }
                }
            }
            gui::text::page::on_mouse_hover(p);
        }

        void on_mouse_leave () override
        {
            if (!touch)
            {
                for (auto & line : view.column)
                {
                    for (auto & token : line)
                    {
                        for (auto & glyph : token)
                        {
                            auto g = glyph.value.now;
                            g.style_index = token.style;
                            glyph.value = g;
                        }
                    }
                }
            }
            gui::text::page::on_mouse_leave();
        }
    };
}