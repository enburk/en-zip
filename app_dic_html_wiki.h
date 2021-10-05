#pragma once
#include "app.h"
namespace app::dic
{
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
        if (ital) html += "</i>";
        if (bold) html += "</b>";
        return html;
    };

    str wiki2html (eng::dictionary::entry entry, array<str> const& links)
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
                    //s.replace_all("CURRENTDAY", );
                    //s.replace_all("CURRENTMONTHNAME", );
                    //s.replace_all("CURRENTYEAR", );

                    str kind = "";
                    str prefix = str(s.upto(4));
                    if (prefix.starts_with("##: ")) { kind = "llc"; s = s.from(4); } else
                    if (prefix.starts_with("#:: ")) { kind = "lcc"; s = s.from(4); } else
                    if (prefix.starts_with("## " )) { kind = "ll";  s = s.from(3); } else
                    if (prefix.starts_with("#: " )) { kind = "lc";  s = s.from(3); } else
                    if (prefix.starts_with("# "  )) { kind = "l";   s = s.from(2); }

                    if (kind == "l" and s.starts_with("(")) {
                        str s1, s2; s.split_by(")", s1, s2, str::delimiter::to_the_left);
                        if (s1 !="" and s2 != "")// and not s1.from(1).contains("("))
                            s = "<font color=#505050>" + s1 + "</font>" + s2;
                    }
                    if (kind == "l" and s.ends_with("]")) {
                        str s1, s2; s.split_by("[", s1, s2, str::delimiter::to_the_right);
                        if (s1 !="" and s2 != "")// and not s2.from(1).contains("["))
                            s = s1 + "<font color=#505050>" + s2 + "</font>";
                    }

                    if (s.starts_with("synonyms: "   ) or s.starts_with("synonym: "    ) or
                        s.starts_with("antonyms: "   ) or s.starts_with("antonym: "    ) or
                        s.starts_with("holonyms: "   ) or s.starts_with("holonym: "    ) or
                        s.starts_with("hyponyms: "   ) or s.starts_with("hyponym: "    ) or
                        s.starts_with("hypernyms: "  ) or s.starts_with("hypernym: "   ) or
                        s.starts_with("meronyms: "   ) or s.starts_with("meronym: "    ) or
                        s.starts_with("troponyms: "  ) or s.starts_with("troponym: "   ) or
                        s.starts_with("homophones: " ) or s.starts_with("homophone: "  ) or
                        s.starts_with("hyphenation: ") or s.starts_with("Hyphenation: ") or
                        s.starts_with("coordinate terms: ") or
                        s.starts_with("coordinate term: ")) {
                        str l, r; s.split_by(":", l, r);
                        s = "<font color=#008000><i>" + l +
                            ":" + "</i></font>" + r; }

                    if (kind == "llc") html += gap + "<div style=\"margin-left: 3em\">"; else
                    if (kind == "lcc") html += gap + "<div style=\"margin-left: 3em\">"; else
                    if (kind == "ll" ) html += gap + "<div style=\"margin-left: 2em\">"; else
                    if (kind == "lc" ) html += gap + "<div style=\"margin-left: 2em\">"; else
                    if (kind == "l"  ) html += gap + "<div style=\"margin-left: 1em\">"; 

                    if (kind == "ll" ) html += gap + std::to_string(n2++) + ". "; else
                    if (kind == "l"  ) html += gap + std::to_string(n1++) + ". ";
                    if (kind == "l"  ) n2 = 1;

                    // for (auto& link : links) s.replace_all(
                    //     link, "<b>" + link + "</b>");
                    
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

                str br = header == "Pronunciation" and
                    topic.content.size() > 1 ?
                    "<br>" : "";

                html += "<div style=\"margin-left: 1em\">";
                html += "<br><i><font color=#008000>"
                     + header+":"+"</font></i> "+br;

                for (str s : topic.content)
                {
                    str kind = "";
                    str prefix = str(s.upto(5));
                    if (prefix.starts_with(":::: ")) { kind = "4"; s = s.from(5); } else
                    if (prefix.starts_with("::: ")) { kind = "3"; s = s.from(4); } else
                    if (prefix.starts_with(":: ")) { kind = "2"; s = s.from(3); } else
                    if (prefix.starts_with(": ")) { kind = "1"; s = s.from(2); } else
                    {}
                    if (kind == "4") html += gap + "<div style=\"margin-left: 4em\">"; else
                    if (kind == "3") html += gap + "<div style=\"margin-left: 3em\">"; else
                    if (kind == "2") html += gap + "<div style=\"margin-left: 2em\">"; else
                    if (kind == "1") html += gap + "<div style=\"margin-left: 1em\">"; else
                    {}
                    
                    if (s.starts_with("homophones: " ) or
                        s.starts_with("homophone: "  ) or
                        s.starts_with("hyphenation: ") or
                        s.starts_with("Hyphenation: ")) {
                        str l, r; s.split_by(":", l, r);
                        s = "<font color=#008000><i>" + l +
                            ":" + "</i></font>" + r; }

                    // for (auto& link : links) s.replace_all(
                    //     link, "<b>" + link + "</b>");
                    
                    html += s + "<br>";
                }

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
                    str ss;
                    for (str s : topic.content)
                    {
                        if (ss != "" and not
                            ss.ends_with(":"))
                            ss += ",";
                        
                        if (ss != "")
                            ss += " ";

                        if (not s.contains("("))    
                        ss += "<a href=\"" + s + "\">"
                            + s + "</a>";
                        else ss += s;
                    }

                    html += ss;
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