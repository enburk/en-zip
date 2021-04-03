#pragma once
#include "app.h"
namespace app::dict
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

    auto bold_italic = [](str s)
    {
        str html;
        int apostrophes = 0;
        bool bold = false;
        bool ital = false;
        s += ' '; // proceed ending apostrophe
        for(char c : s)
        {
            if (c == '\'' ) apostrophes++; else
            {
                if (apostrophes >= 5 ) {
                    apostrophes -= 5;
                    html += bold ? "</b>" : "<b>"; bold = !bold;
                    html += ital ? "</i>" : "<i>"; ital = !ital;
                }
                if( apostrophes >= 3 ) {
                    apostrophes -= 3;
                    html += bold ? "</b>" : "<b>"; bold = !bold;
                }
                if( apostrophes >= 2 ) {
                    apostrophes -= 2;
                    html += ital ? "</i>" : "<i>"; ital = !ital;
                }
                html += str('\'', apostrophes);
                apostrophes = 0;
                html += c;
            }
        }
        return html;
    };

        //struct topic { str header, forms; array<str> content; };
        //struct entry { str title; array<topic> topics; };

    str wiki2html (eng::dictionary::entry entry)
    {
        entry.title.replace_all("&", "&amp;");
        for (auto & topic : entry.topics) {
            topic.header.replace_all("&", "&amp;");
            topic.forms .replace_all("&", "&amp;");
            for (str & s : topic.content)
                s.replace_all("&", "&amp;");
        }

        str html = "<h4>" + entry.title + "</h4>";

        str gap = "<div style=\"line-height: 50%\"><br></div>";

        for (const auto & topic : entry.topics)
        {
            if (eng::lexical_items.find(topic.header) !=
                eng::lexical_items.end())
            {
                html += "<br><b><font color=#008000>"
                        + topic.header + "</font></b> &nbsp; "
                        + topic.forms + "<br>";

                int n1 = 1, n2 = 1;

                for (str s : topic.content)
                {
                    str kind = "";
                    str prefix = "# " + str(s.upto(4));
                    if (prefix.starts_with("# # : ")) { kind = "llc"; s = s.from(4); } else
                    if (prefix.starts_with("# # "  )) { kind = "ll";  s = s.from(2); } else
                    if (prefix.starts_with("# : "  )) { kind = "lc";  s = s.from(2); } else
                    if (prefix.starts_with("# "    )) { kind = "l";   s = s.from(0); } 

                    if (kind == "llc") html += gap + "<div style=\"margin-left: 3em\">"; else
                    if (kind == "ll" ) html += gap + "<div style=\"margin-left: 2em\">"; else
                    if (kind == "lc" ) html += gap + "<div style=\"margin-left: 2em\">"; else
                    if (kind == "l"  ) html += gap + "<div style=\"margin-left: 1em\">"; 

                    if (kind == "ll" ) html += gap + std::to_string(n2++) + ". "; else
                    if (kind == "l"  ) html += gap + std::to_string(n1++) + ". ";
                    if (kind == "l"  ) n2 = 1;

                    html += s + "<br></div>";
                }
            }
            else
            if (eng::lexical_notes.find(topic.header) !=
                eng::lexical_notes.end())
            {
                str header = topic.header;
                if (header.size() > 0)
                    header[0] = header[0] - 'a' + 'A';

                html += "<div style=\"margin-left: 1em\">";
                html += "<br><i><font color=#008000>"
                     + header + ":</font></i> ";

                for (str s : topic.content)
                    html += s + "<br>";

                html += "</div>";
            }
            else
            if (eng::related_items.find(topic.header) !=
                eng::related_items.end())
            {
                str header = topic.header;
                if (header.size() > 0)
                    header[0] = header[0] - 'a' + 'A';

                html += "<div style=\"margin-left: 1em\">";
                html += "<br><i><font color=#008000>"
                     + header + ":</font></i> ";

                if (topic.content.size() >= 10)
                {
                    str ss, sss;
                    for (str s : topic.content)
                    {
                        if (ss.size() + s.size() > 80) {
                            sss += "<br>" + ss;
                            ss = "";
                        }

                        ss += "<a href=\"" + s + "\">"
                            + s + "</a>" + ", ";
                    }
                    sss += "<br>" + ss;

                    html += sss;
                    html.truncate();
                    html.truncate();
                    html += "<br>";
                }
                else
                {
                    if (topic.content.size() >= 2)
                        html += "<br>";

                    for (str s : topic.content)
                        html += "<a href=\"" + s + "\">"
                            + s + "</a>" + "<br>";
                }

                html += "</div>";
            }
        }

        return bold_italic(html);
    }
}