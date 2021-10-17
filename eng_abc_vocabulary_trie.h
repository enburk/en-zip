#pragma once
#include "eng_abc_vocabulary.h"
namespace eng
{
    struct vocabulary_trie : vocabulary_basic
    {
        std::array<int, 27*27*27> trie{};

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

        explicit vocabulary_trie () = default;
        explicit vocabulary_trie (dictionary& dic)
            : vocabulary_basic(dic)
        {
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
        explicit vocabulary_trie (std::filesystem::path path)
            : vocabulary_basic(path)
        {
            dat::in::pool pool(path);
            vocabulary_basic::load(pool);
        }
        void save (dat::out::file& file)
        {
            vocabulary_basic::save(file);
        }
    };
}
