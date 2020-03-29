#include "app_dic_abc.h"
namespace app::dic
{
    struct html_view : gui::text::view
    {
        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            str link;

            if (!page.coord.now.includes(p)) return;
            auto pp = p - page.coord.now.origin;
            for (auto & section : page)
            {
                if (!section.coord.now.includes(pp)) continue;
                auto sp = pp - section.coord.now.origin;
                for (auto & line : section)
                {
                    if (!line.coord.now.includes(sp)) continue;
                    auto lp = sp - line.coord.now.origin;
                    for (auto & token : line)
                    {
                        if (!token.coord.now.includes(lp)) continue;
                        if (token.glyphs.size() < 2) continue;
                        link = token.text;
                        break;
                    }
                }
            }

            auto s = link; s.triml();
            auto r = vocabulary.equal_range(eng::vocabulary::entry{s}, less);
            if (!r) r = vocabulary.equal_range(eng::vocabulary::entry{s}, less_case_insentive);
            if (!r) return;

            notify(r.offset);

        }
        void on_mouse_hover (XY p) override
        {
            str link;

            if (!page.coord.now.includes(p)) return;
            auto pp = p - page.coord.now.origin;
            for (auto & section : page)
            {
                if (!section.coord.now.includes(pp)) continue;
                auto sp = pp - section.coord.now.origin;
                for (auto & line : section)
                {
                    if (!line.coord.now.includes(sp)) continue;
                    auto lp = sp - line.coord.now.origin;
                    for (auto & token : line)
                    {
                        if (!token.coord.now.includes(lp)) continue;
                        if (token.glyphs.size() < 2) continue;
                        link = token.text;
                        break;
                    }
                }
            }

            auto s = link; s.triml();
            auto r = vocabulary.equal_range(eng::vocabulary::entry{s}, less);
            if (!r) r = vocabulary.equal_range(eng::vocabulary::entry{s}, less_case_insentive);
            if (!r) link = "";

            for (auto & section : page)
            {
                for (auto & line : section)
                {
                    for (auto & token : line)
                    {
                        auto style_index = token.style;
                        if (token.text == link)
                        {
                            auto style = style_index.style();
                            style.color = RGBA(0,0,255);
                            style_index = sys::glyph_style_index(style);
                        }

                        for (auto & glyph : token)
                        {
                            auto g = glyph.value.now;
                            g.style_index = style_index;
                            glyph.value = g;
                        }
                    }
                }
            }
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

    str wiki2html (eng::dictionary::entry entry)
    {
        str html = "<h4>" + entry.title + "</h4>";

        str gap = "<div style=\"line-height: 50%\"><br></div>";

        for (const auto & topic : entry.topics)
        {
            if (lexical_items.find(topic.header) != lexical_items.end())
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
            {
                str header = topic.header;
                if (header.size() > 0)
                    header[0] = header[0] - 'a' + 'A';

                html += "<div style=\"margin-left: 1em\">";
                html += "<br><i><font color=#008000>"
                     + header + ":</font></i> ";

                for (str s : topic.content)
                {
                    html += s + "<br>";
                }

                html += "</div>";
            }
        }

        return bold_italic(html);
    }
}