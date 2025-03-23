#pragma once
#include "eng_abc_dictionary.h"
namespace eng
{
    struct vocabulary_basic
    {
        struct entry
        { 
            str title;

            void operator << (sys::in::pool& in)
            {
                in >> title;
            }
            void operator >> (sys::out::file& out) const
            {
                out << title;
            }
        };

        array<entry> data;
        int size () const { return data.size(); }
        entry& operator [] (int i) { return data[i]; }

        std::optional<int> index (str const& s)
        {
            int i = lower_bound(s);
            if (data[i].title != s) return {};
            return i;
        }

        int lower_bound (str const& s)
        {
            auto it = std::ranges::lower_bound
            (data, s, eng::less, &entry::title);
            if (it == data.end()) return data.size()-1;
            return (int)(it - data.begin());
        }

        int lower_bound_case_insensitive (str const& s)
        {
            str lower = lowercased(s);
            str upper = uppercased(s);
            int i = lower_bound(upper);
            while (i < size()-1) {
                if (data[i].title == s or
                    less(s, data[i].title) or
                    less(lower, data[i+1].title))
                    return i;
                i++;
            }
            return i;
        }

        explicit vocabulary_basic () = default;
        explicit vocabulary_basic (dictionary& dic)
        {
            data.resize(dic.entries.size());
            for (int i=0; i<dic.entries.size(); i++)
                data[i].title = dic.entries[i].title;
        }
        explicit vocabulary_basic (std::filesystem::path path)
        {
            sys::in::pool pool(path);
            load(pool);
        }
        void load (sys::in::pool& pool)
        {
            pool.get_endianness();
            data.resize(pool.get_int());
            for (int i=0; i<data.size(); i++)
                data[i] << pool;
        }
        void save (sys::out::file& file)
        {
            file << 0x12345678; // endianness
            file << data.size();
            for (auto& entry: data)
                entry >> file;
        }
    };

    struct vocabulary_hashed : vocabulary_basic
    {
        std::unordered_map<str, int> hashmap;

        std::optional<int> index (str const& s)
        {
            auto it = hashmap.find(s);
            if (it == hashmap.end()) return {};
            return it->second;
        }

        explicit vocabulary_hashed () = default;
        explicit vocabulary_hashed (dictionary& dic)
            : vocabulary_basic(dic)
        {
            hashmap.reserve(data.size());
            for (int i=0; i<data.size(); i++)
                hashmap[data[i].title] = i;
        }
        explicit vocabulary_hashed (std::filesystem::path path)
            : vocabulary_basic(path)
        {
            hashmap.reserve(data.size());
            for (int i=0; i<data.size(); i++)
                hashmap[data[i].title] = i;
        }
    };

    struct vocabulary_cached : vocabulary_basic
    {
        std::vector<int> cache;

        std::optional<int> index (str const& s) const
        {
            int i = lower_bound(s);
            if (data[i].title != s) return {};
            return i;
        }

        bool contains (str const& s) const { return index(s) ? true : false; }

        int lower_bound_case_insensitive (str const& s) const
        {
            str lower = lowercased(s);
            str upper = uppercased(s);
            int i = lower_bound(upper);
            while (i < size()-1) {
                if (data[i].title == s or
                    less(s, data[i].title) or
                    less(lower, data[i+1].title))
                    return i;
                i++;
            }
            return i;
        }

        int lower_bound (str const& s) const
        {
            auto b = data.begin();
            auto e = data.end();

            int n = 0; for (int i=0; i<3; i++)
            {
                char c = i >= s.size() ? ' ' : s[i];

                int j = 
                ' ' == c ? 0 :
                'A' <= c && c <= 'Z' ? c - 'A' + 1 :
                'a' <= c && c <= 'z' ? c - 'a' + 1 :
                    -1;

                if (j == -1) { n = -1; break; }

                n += i == 0 ? j*27*27 :
                     i == 1 ? j*27 : j;
            }

            if (n != -1) {
                b = data.begin() + cache[n]; if (n+1 < cache.size())
                e = data.begin() + cache[n+1];
            }

            auto it = std::lower_bound(b, e, entry{s},
            [] (entry const& e1, entry const& e2)
            { return eng::less(e1.title, e2.title); });
            if (it == data.end()) return data.size()-1;
            return (int)(it - data.begin());
        }

        explicit vocabulary_cached () = default;
        explicit vocabulary_cached (dictionary& dic)
            : vocabulary_basic(dic)
        {
            cache.resize(27*27*27);
            auto t = cache.begin();
            for (int i=0; i<27; i++)
            for (int j=0; j<27; j++)
            for (int k=0; k<27; k++)
            {
                str s;
                s += i == 0 ? ' ' : 'A' + i-1;
                s += j == 0 ? ' ' : 'A' + j-1;
                s += k == 0 ? ' ' : 'A' + k-1;
                s.trimr();
                *t = vocabulary_basic::lower_bound(s);
                ++t;
            }
        }
        explicit vocabulary_cached (std::filesystem::path path)
            : vocabulary_basic(path)
        {
            sys::in::pool pool(path);
            vocabulary_basic::load(pool);
            cache.resize(27*27*27);
            for (int i=0; i<cache.size(); i++)
                cache[i] = pool.get_int();
        }
        void save (sys::out::file& file)
        {
            vocabulary_basic::save(file);
            for (int n: cache)
                file << n;
        }
    };
}

namespace eng { using vocabulary = vocabulary_cached; }

