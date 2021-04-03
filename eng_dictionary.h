#pragma once
#include "eng.h"
#include "dat_in.h"
#include "dat_out.h"
namespace eng
{
    array<str> lexical_items
    {
        "noun", "pronoun", "proper noun",
        "verb", "adjective", "adverb", "numeral", "number", "article", "particle", 
        "preposition", "postposition", "determiner", "conjunction", "interjection",
        "initialism", "contraction", "acronym", 
        "letter", "symbol", "punctuation mark", "diacritical mark",
        "suffix", "prefix", "infix", "affix", "interfix",
        "phrase", "prepositional phrase", "proverb", 
    };
    array<str> related_items
    {
        "abbreviations", "alternative forms", "synonyms", "antonyms",
        "hypernyms", "hyponyms", "meronyms", "holonyms", "troponyms",
        "derived terms", "related terms", "coordinate terms",
        "see also", 
    };
    array<str> lexical_notes
    {
        "pronunciation", "etymology",
        "usage notes", "trivia",
    };

    namespace dictionary
    {
        struct topic { str header, forms; array<str> content; };
        struct entry { str title; array<topic> topics; array<int> redirects; };

        auto less = [](const entry & a, const entry & b)
        { return eng::less(a.title, b.title); };

        auto less_case_insensitive = [](const entry & a, const entry & b)
        { return eng::less_case_insensitive(a.title, b.title); };

        bool operator < (const entry & a, const entry & b)
        { return eng::less(a.title, b.title); };

        void operator >> (dat::in::pool & in, entry & entry)
        {
            entry.title = in.get_string();
            entry.topics.resize(in.get_int());
            for (auto & topic : entry.topics) {
                topic.header = in.get_string();
                topic.forms  = in.get_string();
                topic.content.resize(in.get_int());
                for (auto & s : topic.content) s = in.get_string();
            }
            entry.redirects.resize(in.get_int());
            for (auto & s : entry.redirects) s = in.get_int();
        }
        void operator << (dat::out::pool & out, const entry & entry)
        {
            out << entry.title;
            out << entry.topics.size();
            for (auto & topic : entry.topics) {
                out << topic.header;
                out << topic.forms;
                out << topic.content;
            }
            out << entry.redirects.size();
            for (auto & redirect : entry.redirects) {
                out << redirect;
            }
        }
    }

    namespace vocabulary
    {
        struct entry { str title; int offset = 0, length = 0, redirect = -1; };
        inline array<entry> data;

        auto less = [](const entry & a, const entry & b)
        { return eng::less(a.title, b.title); };

        auto less_case_insensitive = [](const entry & a, const entry & b)
        { return eng::less_case_insensitive(a.title, b.title); };

        bool operator < (const entry & a, const entry & b)
        { return eng::less(a.title, b.title); };

        auto find (str s)
        {
            auto e = eng::vocabulary::entry{s};
            auto range = data.equal_range(e, less);
            return range;
        }

        auto find_case_insensitive (str s)
        {
            auto e = eng::vocabulary::entry{s};
            auto range = data.equal_range(e, less); if (range.size() == 0)
                 range = data.equal_range(e, less_case_insensitive);
            return range;
        }

        void operator >> (dat::in::pool & in, entry & entry)
        {
            entry.title = in.get_string();
            entry.offset = in.get_int();
            entry.length = in.get_int();
            entry.redirect = in.get_int();
        }
        void operator << (dat::out::pool & out, const entry & entry)
        {
            out << entry.title;
            out << entry.offset;
            out << entry.length;
            out << entry.redirect;
        }
    }
}
