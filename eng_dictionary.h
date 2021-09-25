#pragma once
#include "eng.h"
#include "data_in.h"
#include "data_out.h"
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

            void operator << (dat::in::pool& in)
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
            void operator >> (dat::out::file& out) const
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

        array<entry> data;
        entry& operator [] (int i)
            { return data[i]; }
    };

    struct vocabulary
    {
        struct entry
        { 
            str title;
            int offset = 0;
            int length = 0;
            int redirect = -1;

            void operator << (dat::in::pool& in)
            {
                title = in.get_string();
                offset = in.get_int();
                length = in.get_int();
                redirect = in.get_int();
            }
            void operator >> (dat::out::file& out) const
            {
                out << title;
                out << offset;
                out << length;
                out << redirect;
            }
        };

        array<entry> data;
        int size () const { return data.size(); }
        entry& operator [] (int i) { return data[i]; }
        std::unordered_map<str, int> hashmap;
        std::array<int, 27*27*27> trie{};
        vocabulary () = default;

        std::optional<int> index (str const& s)
        {
            auto it = hashmap.find(s);
            if (it == hashmap.end()) return {};
            return it->second;
        }

        int lower_bound (str const& s)
        {
            auto it = data.lower_bound(entry{s},
            [] (entry const& a, entry const& b)
            { return eng::less(a.title, b.title); });
            if (it == data.end()) return data.size()-1;
            return (int)(it - data.begin());
        }
        //int lower_bound_ex (str const& s)
        //{
        //    int n = lower_bound(s);
        //    while (n+1 < size() and
        //        not eng::less(data[n+1]))
        //}

        explicit vocabulary (dictionary& dic)
        {
            data.resize(dic.data.size());
            hashmap.reserve(dic.data.size());
            for (int i=0; i<dic.data.size(); i++) {
                data[i].title = dic.data[i].title;
                hashmap[data[i].title] = i;
            }
            auto t = trie.begin();
            for (int i=0; i<27; i++)
            for (int j=0; j<27; j++)
            for (int k=0; k<27; k++)
            {
                str s;
                s += ' ' + i;
                s += ' ' + j;
                s += ' ' + k;
                *t = lower_bound(s);
                ++t;
            }
        }
        explicit vocabulary (std::filesystem::path path)
        {
            dat::in::pool pool(path);
            pool.get_endianness();
            data.resize(pool.get_int());
            hashmap.reserve(data.size());
            for (int i=0; i<data.size(); i++) {
                data[i] << pool;
                hashmap[data[i].title] = i;
            }
        }



        static auto less (const entry & a, const entry & b)
        { return eng::less(a.title, b.title); };

        static auto less_case_insensitive (const entry & a, const entry & b)
        { return eng::less_case_insensitive(a.title, b.title); };

        auto find_case_insensitive (str s)
        {
            auto e = eng::vocabulary::entry{s};
            auto r1 = data.equal_range(e, less); if (r1) return r1;
            auto r2 = data.equal_range(e, less_case_insensitive);
            return r2;
        }
    };
}
