#pragma once
#include "app.h"
namespace app::dic
{
    str wiki2html (eng::dictionary::entry entry, array<str> const& links)
    {
        entry.title.replace_all("&", "&amp;");
        for (auto & topic : entry.topics) {
            topic.header.replace_all("&", "&amp;");
            topic.forms .replace_all("&", "&amp;");
            for (str & s : topic.content)
                s.replace_all("&", "&amp;");
        }

        str html;
        html.reserve(16*1024);
        html += "<h4>" + entry.title + "</h4>";

        str gap = "<div style=\"line-height: 50%\"><br></div>";

        try {
        for (auto& topic : entry.topics)
        {
            if (eng::lexical_items.contains(topic.header))
            {
                if (topic.forms.size() > 100)
                {
                    if (array<str> ff = topic.forms.split_by(";"); ff.size() > 1)
                    {
                        bool split = true;
                        for (str f: ff) if (f.size() < 20) split = false;
                        if (split)
                        {
                            topic.forms = ""; for (str f: ff)
                            {
                                if (topic.forms != "")
                                    topic.forms += ";<br>"
                                    "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                                    "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";

                                topic.forms += f;
                            }
                        }
                    }
                }

                html += "<br>" + green(bold(topic.header)) +
                    " &nbsp; " + bold_italic(topic.forms) + "<br>";

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

                    if (kind == "l" or
                        kind == "ll")
                    {
                        if (s.starts_with("(")) {
                            str s1, s2; s.split_by(")", s1, s2, str::delimiter::to_the_left);
                            if (s1 !="" and s2 != "" and not s1.from(1).contains("("))
                                s = dark(s1) + s2;
                        }
                        if (s.ends_with("]</small>")) {
                            str s1, s2; s.split_by("<small>[", s1, s2, str::delimiter::to_the_right);
                            if (s1 !="" and s2 != "" and not s2.from(8).contains("["))
                                s = s1 + dark(s2);
                        }
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
                            s = green(italic(l + ":")) + r; }

                    str indent;
                    if (kind == "ll" )
                    {
                        str n = std::to_string(n2);
                        str m = std::to_string(n.size()+1);
                        indent += "; margin-left: "+m+"ch";
                        indent += "; text-indent:-"+m+"ch";
                    }
                    if (kind == "l"  )
                    {
                        str n = std::to_string(n1);
                        str m = std::to_string(n.size()+1);
                        indent += "; margin-left: "+m+"ch";
                        indent += "; text-indent:-"+m+"ch";
                    }

                    if (kind == "llc") html += gap + "<div style=\"margin-left: 3em"+indent+"\">"; else
                    if (kind == "lcc") html += gap + "<div style=\"margin-left: 3em"+indent+"\">"; else
                    if (kind == "ll" ) html += gap + "<div style=\"margin-left: 2em"+indent+"\">"; else
                    if (kind == "lc" ) html += gap + "<div style=\"margin-left: 2em"+indent+"\">"; else
                    if (kind == "l"  ) html += gap + "<div style=\"margin-left: 1em"+indent+"\">"; 

                    if (kind == "ll" ) html += gap + std::to_string(n2++) + ". "; else
                    if (kind == "l"  ) html += gap + std::to_string(n1++) + ". ";
                    if (kind == "l"  ) n2 = 1;
                    
                    html += bold_italic(s) + "<br></div>";
                }
            }
            else
            if (eng::lexical_notes.contains(topic.header))
            {
                str header = topic.header;
                if (header.size() > 0)
                    header[0] = header[0] - 'a' + 'A';

                html += "<br>";
                html += "<div style=\"margin-left: 1em\">";
                html += green(italic(header + ": ")) + 

                   (header == "Pronunciation" and
                    topic.content.size() > 1 ?
                    "<br>" : "");

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
                        s = green(italic(l + ":")) + r; }
                    
                    html += bold_italic(s) + "<br>";

                    if (kind == "4"
                    or  kind == "3"
                    or  kind == "2"
                    or  kind == "1")
                    html += "</div>";
                }

                html += "</div>";
            }
            else
            if (eng::related_items.contains(topic.header))
            {
                str header = topic.header;
                if (header.size() > 0)
                    header[0] = header[0] - 'a' + 'A';

                html += "<br>";
                html += "<div style=\"margin-left: 1em\">";
                html += green(italic(header + ": "));

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

                        ss += s;
                    }

                    html += bold_italic(ss);
                    html += "<br>";
                }
                else
                {
                    if (topic.content.size() >= 2)
                        html += "<br>";

                    for (str s : topic.content)
                        html += bold_italic(s) +
                            "<br>";
                }

                html += "</div>";
            }
        }}
        catch(std::runtime_error const& e)
        {
            if (str(e.what()).starts_with("unicode"))
            html += "<br>" + red(e.what());
            else throw;
        }

        return html;
    }
}