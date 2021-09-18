#pragma once
#include "app.h"
namespace app::dic
{
    struct html_view : gui::text::page
    {
        int clicked = 0;

        array<str> excluded_links; bool it_is_a_title = false;

        int link (gui::text::token & token, gui::text::line & line)
        {
            if (token.info != "")
                if (auto range =
                    eng::vocabulary::find_case_insensitive(token.info);
                    not range.empty()) return range.offset();

            int index = -1;
            for (int i=0; i<line.size(); i++)
                if (&line(i) == &token) {
                    index = i; break; }

            if (index == -1) return eng::vocabulary::find("nonsense").offset();

            auto small = [](str s){ return eng::asciized(s).ascii_lowercased(); };

            str Longest; int L = 0; int I0 = 0; int I1 = 0;
            str longest; int l = 0; int i0 = 0; int i1 = 0;

            for (int start=0; start<=index; start++)
            {
                str Attempt = "";
                str attempt = "";

                for (int i=start; i<=index; i++) {
                    Attempt += line(i).text;
                    attempt += small(line(i).text);
                }

                auto it = vocabulary.lower_bound(
                     eng::vocabulary::entry{attempt},
                     eng::vocabulary::less_case_insensitive);

                if (it == vocabulary.end() or not small(
                    it->title).starts_with(attempt))
                    continue;

                str S = it->title;
                str s = small(S);
                int d = (int)(it - vocabulary.begin());
                bool Better = S == Attempt and Longest.size() < S.size();
                bool better = s == attempt and longest.size() < s.size();
                Better = Better and not excluded_links.contains(S);
                better = better and not excluded_links.contains(S);
                if (Better) { Longest = S; L = d; I0 = start; I1 = index; }
                if (better) { longest = S; l = d; i0 = start; i1 = index; }

                str sentinel = attempt;

                for (int i=index+1; i<line.size(); i++)
                {
                    bool valid = false;
                    str Candidate = Attempt + line(i).text;
                    str candidate = attempt + small(line(i).text);
                    for (auto jt=it; it != vocabulary.end(); jt++)
                    {
                        S = jt->title;
                        s = small(S);
                        int d = (int)(jt - vocabulary.begin());
                        Better = S == Candidate and Longest.size() < S.size();
                        better = s == candidate and longest.size() < s.size();
                        Better = Better and not excluded_links.contains(S);
                        better = better and not excluded_links.contains(S);
                        if (Better) { Longest = S; L = d; I0 = start; I1 = i; }
                        if (better) { longest = S; l = d; i0 = start; i1 = i; }

                        if (s.starts_with(candidate) or
                            candidate.starts_with(s))
                            valid = true;

                        if (not s.starts_with(sentinel))
                            break;
                    }
                    if (not valid) break;
                    Attempt = Candidate;
                    attempt = candidate;
                }
            }

            str c = Longest.upto(1); bool capital =
                c >= "A" and
                c <= "Z";

            if (Longest.size() <  longest.size() or
               (Longest.size() == longest.size() and capital and it_is_a_title)) {
                Longest = longest; L = l; I0 = i0; I1 = i1; }

            if (Longest != "")
            {
                for (int i=I0; i<=I1; i++)
                    line(i).info = vocabulary[L].title;

                return L;
            }

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
                    if (token.glyphs.size() < 2) continue;
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
                if (auto n = link(p); n >= 0)
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
                int n = link(p); mouse_image =
                    n == -1 ? "arrow":
                    n == -2 ? "arrow":
                    "hand";

                str link = n >= 0 ?
                    vocabulary[n].title : "";

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