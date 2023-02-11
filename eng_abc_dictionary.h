#pragma once
#include "eng_abc.h"
namespace eng
{
    const array<str> lexical_items
    {
        "noun", "pronoun", "proper noun",
        "verb", "adjective", "adverb", "numeral", "number", "article", "particle", 
        "preposition", "postposition", "determiner", "conjunction", "interjection",
        "initialism", "contraction", "acronym", 
        "letter", "symbol", "punctuation mark", "diacritical mark",
        "suffix", "prefix", "infix", "affix", "interfix",
        "phrase", "prepositional phrase", "proverb", 
    };
    const array<str> related_items
    {
        "abbreviations", "alternative forms", "synonyms", "antonyms",
        "hypernyms", "hyponyms", "meronyms", "holonyms", "troponyms",
        "derived terms", "related terms", "coordinate terms",
        "see also", 
    };
    const array<str> lexical_notes
    {
        "pronunciation", "etymology",
        "usage notes", "trivia",
    };

    struct dictionary
    {
        struct topic {
            str header, forms;
            array<str> content;
        };

        struct entry {
            str title;
            array<topic> topics;
            array<int> redirects;

            void operator << (sys::in::pool& in)
            {
                title = in.get_string();
                topics.resize(in.get_int());
                for (auto & topic : topics) {
                    topic.header = in.get_string();
                    topic.forms  = in.get_string();
                    topic.content.resize(in.get_int());
                    for (auto & s : topic.content)
                        s = in.get_string();
                }
                redirects.resize(in.get_int());
                for (auto & s : redirects)
                    s = in.get_int();
            }
            void operator >> (sys::out::file& out) const
            {
                out << title;
                out << topics.size();
                for (auto & topic : topics) {
                    out << topic.header;
                    out << topic.forms;
                    out << topic.content;
                }
                out << redirects.size();
                for (auto & redirect : redirects)
                    out << redirect;
            }
        };

        struct index
        { 
            int offset = 0;
            int length = 0;
            int redirect = -1;

            inline static const int size = 3*4;

            void operator << (sys::in::pool& pool)
            {
                pool >> offset;
                pool >> length;
                pool >> redirect;
            }
            void operator >> (sys::out::file& pool) const
            {
                pool << offset;
                pool << length;
                pool << redirect;
            }
        };

        array<entry> entries;
        array<index> indices;
    };
}
