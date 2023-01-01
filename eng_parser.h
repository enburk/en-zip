#pragma once
#include "eng_abc_vocabulary_trie.h"
namespace eng { using vocabulary = vocabulary_cached; }
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

    template<class Vocabulary>
    generator<match> matches (Vocabulary& voc, array<token>& tokens)
    {
        auto upper = [](token const& t){ return t.text; };
        auto lower = [](token const& t){ return
            t.kind == "Text" or t.text == "I" ?
            t.text : lowercased(t.text); };

        for (int t=0; t<tokens.size(); t++)
        {
            str TexT = upper(tokens[t]); if (TexT == " ") continue;
            str Text = lower(tokens[t]);
            str text = lowercased(Text);
            int tt = 1;

            int i = voc.lower_bound(TexT);
            str Title = voc[i].title;
            str title = lowercased(Title);

            while (true)
            {
                if (Title == Text or
                    Title == TexT)
                {
                    co_yield {t, tt, i, Title};

                    if (Text != TexT)
                    {
                        do
                        {
                            i++;
                            if (i >= voc.size()) break;
                            Title = voc[i].title;
                            title = lowercased(Title);
                            if (Title == Text)
                                co_yield {t, tt, i, Title};
                        }
                        while (less(Title, text));
                    }
                }

                if ((text == title) or
                    (text.size() < title.size()
                        and less(text, title)))
                {
                    if (t+tt >= tokens.size()) break;
                    if (tokens[t+tt].text == " ")
                    {
                        tt++;
                        if (t+tt >= tokens.size()) break;
                        TexT += " ";
                        Text += " ";
                        text += " ";
                    }
                    TexT += upper(tokens[t+tt]);
                    Text += lower(tokens[t+tt]);
                    text += lowercased(tokens[t+tt].text);
                    tt++;
                    continue;
                }
                else if (Title.size() <= 4)
                {
                    int j = voc.lower_bound(TexT);
                    if (j <= i) break; i = j;
                    Title = voc[i].title;
                }
                else do
                {
                    i++;
                    if (i >= voc.size()) break;
                    Title = voc[i].title;
                }
                while (less(Title, TexT));

                title = lowercased(Title);
                if (not title.starts_with(text)
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

    array<str> entries (eng::vocabulary& voc, str input, bool casesensitive)
    {
        array<str> entries;

        for (auto line: input.split_by("<br>"))
        {
            array<token> tokens;
            auto text = doc::text::text(line); // save temporary
            for (auto t: doc::html::lexica::parse(text))
            {
                if (t.kind == "text")
                {
                    if (casesensitive) t.kind = "Text";
                    if (t.text == (char*)(u8"’")) t.text = "'"; else
                    if (t.text.starts_with("_")) {
                        t.text.erase(0);
                        t.kind = "Text";
                    }
                    tokens += t;
                }
            }

            match best {-1, 0};

            for (auto m: matches(voc, tokens))
            {
                if (m.text == ""
                or (m.text == "/")
                or (m.text == "."   and input.size() > 3*5)
                or (m.text == ","   and input.size() > 2*5)
                or (m.text == ":"   and input.size() > 9*5)
                or (m.text == ";"   and input.size() > 9*5)
                or (m.text == "!"   and input.size() > 2*5)
                or (m.text == "?"   and input.size() > 2*5)
                or (m.text == "("   and input.size() > 8*5)
                or (m.text == ")"   and input.size() > 8*5)
                or (m.text == "'"   and input.size() > 2*5)
                or (m.text == "\""  and input.size() > 9*5)
                or (m.text == lquot and input.size() > 9*5)
                or (m.text == rquot and input.size() > 9*5)
                or (m.text == "-"   and input.size() > 7*5)
                or (m.text == "--"  and input.size() > 7*5)
                or (m.text == ndash and input.size() > 9*5)
                or (m.text == mdash and input.size() > 9*5)
                or (m.text == "..." and input.size() > 9*5)
                or (m.text == "1"   and input.size() > 8*5)
                or (m.text == "2"   and input.size() > 8*5)
                or (m.text == "3"   and input.size() > 9*5)

                or (m.text == "a"   and input.size() > 2*5)
                or (m.text == "an"  and input.size() > 4*5)
                or (m.text == "the" and input.size() > 3*5)
                or (m.text == "s"   and input.size() > 3*5)
                or (m.text == "t"   and input.size() > 4*5)
                or (m.text == "m"   and input.size() > 4*5)
                or (m.text == "'s"  and input.size() > 3*5)
                or (m.text == "'t"  and input.size() > 4*5)
                or (m.text == "'m"  and input.size() > 4*5)

                or (m.text == "I"    and input.size() > 2*5)
                or (m.text == "I'm"  and input.size() > 9*5)
                or (m.text == "he"   and input.size() > 9*5)
                or (m.text == "his"  and input.size() > 9*5)
                or (m.text == "it"   and input.size() > 3*5)
                or (m.text == "it's" and input.size() > 9*5)
                or (m.text == "me"   and input.size() > 8*5)
                or (m.text == "my"   and input.size() > 9*5)
                or (m.text == "they" and input.size() > 9*5)
                or (m.text == "You"  and input.size() > 9*5)
                or (m.text == "you"  and input.size() > 3*5)
                or (m.text == "your" and input.size() > 8*5)
                or (m.text == "we"   and input.size() > 5*5)

                or (m.text == "about"  and input.size() > 9*5)
                or (m.text == "all"    and input.size() > 5*5)
                or (m.text == "and"    and input.size() > 4*5)
                or (m.text == "are"    and input.size() > 8*5)
                or (m.text == "as"     and input.size() > 9*5)
                or (m.text == "at"     and input.size() > 9*5)
                or (m.text == "be"     and input.size() > 8*5)
                or (m.text == "but"    and input.size() > 5*5)
                or (m.text == "by"     and input.size() > 9*5)
                or (m.text == "can"    and input.size() > 9*5)
                or (m.text == "do"     and input.size() > 3*5)
                or (m.text == "don"    and input.size() > 8*5)
                or (m.text == "don'"   and input.size() > 8*5)
                or (m.text == "don't"  and input.size() > 8*5)
                or (m.text == "for"    and input.size() > 9*5)
                or (m.text == "from"   and input.size() > 9*5)
                or (m.text == "get"    and input.size() > 9*5)
                or (m.text == "go"     and input.size() > 9*5)
                or (m.text == "good"   and input.size() > 9*5)
                or (m.text == "have"   and input.size() > 5*5)
                or (m.text == "how"    and input.size() > 9*5)
                or (m.text == "if"     and input.size() > 9*5)
                or (m.text == "in"     and input.size() > 4*5)
                or (m.text == "is"     and input.size() > 4*5)
                or (m.text == "just"   and input.size() > 9*5)
                or (m.text == "know"   and input.size() > 9*5)
                or (m.text == "like"   and input.size() > 9*5)
                or (m.text == "man"    and input.size() > 9*5)
                or (m.text == "of"     and input.size() > 3*5)
                or (m.text == "oh"     and input.size() > 8*5)
                or (m.text == "on"     and input.size() > 8*5)
                or (m.text == "one"    and input.size() > 9*5)
                or (m.text == "or"     and input.size() > 9*5)
                or (m.text == "out"    and input.size() > 9*5)
                or (m.text == "no"     and input.size() > 3*5)
                or (m.text == "not"    and input.size() > 8*5)
                or (m.text == "now"    and input.size() > 9*5)
                or (m.text == "say"    and input.size() > 9*5)
                or (m.text == "so"     and input.size() > 5*5)
                or (m.text == "that"   and input.size() > 3*5)
                or (m.text == "that's" and input.size() > 8*5)
                or (m.text == "there"  and input.size() > 9*5)
                or (m.text == "this"   and input.size() > 9*5)
                or (m.text == "time"   and input.size() > 9*5)
                or (m.text == "to"     and input.size() > 3*5)
                or (m.text == "up"     and input.size() > 8*5)
                or (m.text == "was"    and input.size() > 9*5)
                or (m.text == "what"   and input.size() > 3*5)
                or (m.text == "when"   and input.size() > 9*5)
                or (m.text == "who"    and input.size() > 9*5)
                or (m.text == "with"   and input.size() > 8*5)
                or (m.text == "will"   and input.size() > 9*5)
                ) continue;

                if (best.token  != m.token or
                    best.tokens != m.tokens)
                    if (best.text != "")
                        entries += best.text;

                best = m;
            }
            if (best.text != "")
                entries += best.text;
        }

        entries.deduplicate();
        return entries;
    }

    str embolden (str input, array<str> entries)
    {
        if (entries.empty()) return input;

        std::ranges::sort(entries, less);

        dictionary dic;
        dic.entries.reserve(entries.size()); for (auto& e : entries)
        dic.entries += dictionary::entry{e};
        vocabulary_basic voc{dic};

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
