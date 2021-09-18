#pragma once
#include "eng_dictionary.h"
namespace eng::parser
{
    array<str> entries (str input)
    {
        array<str> entries;

        input = doc::html::untagged(input);

        for (auto line : input.split_by("\n"))
        {
            auto tokens = doc::html::lexica::tokenize(doc::text::text(line));

            for (int i=0; i<tokens.size(); i++)
            for (int j=i; j<tokens.size(); j++)
            {
                if (tokens[i].text == " ") break;

                str S, s;
                for (int k=i; k<=j; k++)
                {
                    auto t = tokens[k].text;
                    if  (t.starts_with("_"))
                         t.upto(1).erase(); else
                         t = t.ascii_lowercased();

                    s += t.ascii_lowercased();
                    S += t;
                }

                auto it =
                    vocabulary::data.lower_bound(
                    vocabulary::entry{S},
                    vocabulary::less);

                if (it != vocabulary::data.end() and
                    it->title == S) {
                    entries += it->title;
                    continue;
                }

                it =
                    vocabulary::data.lower_bound(
                    vocabulary::entry{s},
                    vocabulary::less_case_insensitive);

                if (it == vocabulary::data.end() or not
                    it->title.ascii_lowercased().
                    starts_with(s))
                    break;
            }
        }

        entries.deduplicate();

        return entries;
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
