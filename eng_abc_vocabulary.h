#pragma once
#include "eng_abc_dictionary.h"
namespace eng
{
    struct vocabulary_
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
    };

    struct vocabulary_basic : vocabulary_
    {
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

        explicit vocabulary_basic () = default;
        explicit vocabulary_basic (dictionary& dic)
        {
            data.resize(dic.data.size());
            for (int i=0; i<dic.data.size(); i++)
                data[i].title = dic.data[i].title;
        }
        explicit vocabulary_basic (std::filesystem::path path)
        {
            dat::in::pool pool(path);
            load(pool);
        }
        void load (dat::in::pool& pool)
        {
            pool.get_endianness();
            data.resize(pool.get_int());
            for (int i=0; i<data.size(); i++)
                data[i] << pool;
        }
        void save (dat::out::file& file)
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

    using vocabulary = vocabulary_hashed;
}
