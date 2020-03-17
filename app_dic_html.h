#include "app_dic_abc.h"
namespace app::dic
{
    auto bold_italic = [](str s)
    {
        str html;
        str apostrophes;
        bool bold = false;
        bool ital = false;
        s += ' '; // proceed ending apostrophe
        for(char c : s)
        {
            if (c == '\'' ) apostrophes += '\''; else
            {
                if (apostrophes.size () >= 5 ) {
                    html += bold ? "</b>" : "<b>"; bold = !bold;
                    html += ital ? "</i>" : "<i>"; ital = !ital;
                    apostrophes.truncate(); apostrophes.truncate();
                    apostrophes.truncate(); apostrophes.truncate();
                    apostrophes.truncate();
                }
                if( apostrophes.size () >= 3 ) {
                    html += bold ? "</b>" : "<b>"; bold = !bold;
                    apostrophes.truncate(); 
                    apostrophes.truncate();
                    apostrophes.truncate();
                }
                if( apostrophes.size () >= 2 ) {
                    html += ital ? "</i>" : "<i>"; ital = !ital;
                    apostrophes.truncate(); 
                    apostrophes.truncate();
                }
                html += apostrophes;
                apostrophes = "";
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

                    if (kind == "llc") html += gap + "<div style=\"margin-left: 9em\">"; else
                    if (kind == "ll" ) html += gap + "<div style=\"margin-left: 6em\">"; else
                    if (kind == "lc" ) html += gap + "<div style=\"margin-left: 6em\">"; else
                    if (kind == "l"  ) html += gap + "<div style=\"margin-left: 3em\">"; 

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

                html += "<div style=\"margin-left: 3em\">";
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