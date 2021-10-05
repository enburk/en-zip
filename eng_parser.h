#pragma once
#include "eng_dictionary.h"
namespace eng::parser
{
    using token = doc::text::token;

    struct match
    {
        int token;
        int tokens;
        int index;
        str text;
    };

    array<match> matches (vocabulary& voc, array<token>& tokens)
    {
        array<match> matches; matches.reserve(2*tokens.size());

        for (int t=0; t<tokens.size(); t++)
        {
            str S = tokens[t].text;
            str s = lowercased(S);
            int tt = 1;

            int i = voc.lower_bound(S);
            str Title = voc[i].title;
            str title = lowercased(Title);

            while (true)
            {
                if (Title == S) matches += {t, tt, i, S}; if (S != s)
                if (Title == s) matches += {t, tt, i, s};

                if (less(s, title))
                {
                    if (t+tt >= tokens.size()) break;
                    str text = tokens[t+tt].text;
                    s += lowercased(text);
                    S += text;
                    tt++;
                    if (text == " ")
                    {
                        if (t+tt >= tokens.size()) break;
                        text = tokens[t+tt].text;
                        s += lowercased(text);
                        S += text;
                        tt++;
                    }
                    continue;
                }
                else do
                {
                    i++;
                    if (i >= voc.size()) break;
                    Title = voc[i].title;
                }
                while (less(Title, S));

                title = lowercased(Title);
                if (not title.starts_with(s))
                    break;
            }
        }

        return matches;
    }

    array<str> entries (eng::vocabulary& voc, str input)
    {
        array<str> Matches, Entries, entries;

        for (auto line: input.split_by("<br>"))
        {
            array<token> tokens;
            auto text = doc::text::text(line); // save temporary
            for (auto t: doc::html::lexica::parse(text))
            {
                if (t.kind == "text")
                {
                    if (t.text.starts_with("_")) {
                        t.text.erase(0);
                        t.kind = "Text";
                    }
                    tokens += t;
                }
            }

            for (auto m: matches(voc, tokens))
            {
                str Text = m.text;
                str text = lowercased(Text);
                if (text == ""
                or (text == "."   and input.size() > 3*5)
                or (text == ","   and input.size() > 3*5)
                or (text == ":"   and input.size() > 9*5)
                or (text == ";"   and input.size() > 9*5)
                or (text == "!"   and input.size() > 5*5)
                or (text == "?"   and input.size() > 5*5)
                or (text == "("   and input.size() > 9*5)
                or (text == ")"   and input.size() > 9*5)
                or (text == "'"   and input.size() > 9*5)
                or (text == "\""  and input.size() > 9*5)
                or (text == lquot and input.size() > 9*5)
                or (text == rquot and input.size() > 9*5)
                or (text == "-"   and input.size() > 9*5)
                or (text == "--"  and input.size() > 9*5)
                or (text == ndash and input.size() > 9*5)
                or (text == "..." and input.size() > 9*5)
                ) continue;

                if((Text != text and tokens[m.token].kind != "Text")
                or (Text == text and tokens[m.token].kind == "Text"))
                    Matches += Text; else {
                    Entries += Text; 
                    entries += text;
                }
            }
        }

        Entries.deduplicate();
        entries.deduplicate();

        for (auto Text: Matches)
            if (not entries.contains(lowercased(Text))) {
                entries += lowercased(Text);
                Entries += Text; }

        return Entries;
    }

    str embolden (str s, array<str> entries)
    {
        entries.sort([](auto a, auto b){
            return a.size() > b.size(); });

        for (auto entry : entries)
            s.replace_all(entry, str(
                "<b>" + entry + "</b>"));

        return s;
    }
}
