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

    generator<match> matches (vocabulary& voc, array<token>& tokens)
    {
        auto upper = [](token const& t){ return t.text; };
        auto lower = [](token const& t){ return
            t.kind == "Text" or t.text == "I" ?
            t.text : lowercased(t.text); };

        for (int t=0; t<tokens.size(); t++)
        {
            str S = upper(tokens[t]); if (S == " ") continue;
            str s = lower(tokens[t]);
            int tt = 1;

            int i = voc.lower_bound(S);
            str Title = voc[i].title;
            str title = lowercased(Title);

            while (true)
            {
                if (Title == s or
                    Title == S)
                {
                    co_yield {t, tt, i, Title};

                    if (S != s)
                    {
                        do
                        {
                            i++;
                            if (i >= voc.size()) break;
                            Title = voc[i].title;
                            title = lowercased(Title);
                            if (Title == s)
                                co_yield {t, tt, i, Title};
                        }
                        while (less(title, s));
                    }
                }

                if ((s == title) or
                    (s.size() < title.size() and
                        less(s, title)))
                {
                    if (t+tt >= tokens.size()) break;
                    if (tokens[t+tt].text == " ")
                    {
                        tt++;
                        if (t+tt >= tokens.size()) break;
                        S += " ";
                        s += " ";
                    }
                    S += upper(tokens[t+tt]);
                    s += lower(tokens[t+tt]);
                    tt++;
                    continue;
                }
                else if (Title.size() <= 4)
                {
                    i = voc.lower_bound(S);
                    Title = voc[i].title;
                }
                else do
                {
                    i++;
                    if (i >= voc.size()) break;
                    Title = voc[i].title;
                }
                while (less(Title, S));

                title = lowercased(Title);
                if (not title.starts_with(s)
                    or i >= voc.size())
                    break;
            }
        }
    }

    void proceed (vocabulary& voc, array<token>& tokens, array<str>& excludes)
    {
        match best {-1, 0};

        for (auto m: matches(voc, tokens))
        {
            if (best.token != m.token)
                for (int t=0; t<best.tokens; t++)
                    tokens[best.token + t].info =
                        best.text;

            if (best.token == m.token or
                best.token + best.tokens <= m.token)
                if (not excludes.contains(m.text))
                    best = m;
        }

        for (int t=0; t<best.tokens; t++)
            tokens[best.token + t].info =
                best.text;
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

    str embolden (str input, array<str> entries)
    {
        std::ranges::sort(entries, less);

        dictionary dic;
        dic.data.reserve(entries.size()); for (auto& e : entries)
        dic.data += dictionary::entry{e};
        vocabulary voc{dic};

        array<token> all;
        array<token> texts;
        std::map<int, int> tokenmap;
        auto text = doc::text::text(input); // save temporary
        for (auto t: doc::html::lexica::parse(text))
        {
            if (t.kind == "text")
            {
                if (t.text.starts_with("_")) {
                    t.text.erase(0);
                    t.kind = "Text";
                }
                tokenmap[texts.size()] = all.size();
                texts += t;
            }
            all += t;
        }

        for (auto m: matches(voc, texts))
        {
            for (int i=0; i<m.tokens; i++)
            {
                int j = tokenmap[m.token + i];
                all[j].text = "<b>" + all[j].text + "</b>";
            }
        }

        str output;
        for (auto t: all)
            output += t.text;

        return output;
    }
}
