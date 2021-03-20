#pragma once
#include "eng.h"
#include <fstream>
#include <filesystem>
namespace dat::out
{
    using std::byte;
    using std::filesystem::path;

    struct pool
    {
        array<byte> bytes;

        pool& operator << (int32_t n)
        {
            int size = sizeof(n);
            byte* data = (byte*)(&n);
            bytes.container::insert(bytes.end(), data, data+size);
            return *this;
        }
        pool& operator << (const str & s)
        {
            *this << s.size();
            int size = s.size();
            byte* data = (byte*)(s.c_str()); 
            bytes.container::insert(bytes.end(), data, data+size);
            return *this;
        }
        template<class T>
        pool& operator << (const array<T> & a)
        {
            *this << a.size();
            int size = a.size()*sizeof(T);
            byte* data = (byte*)(a.data());
            bytes.insert(bytes.end(), data, data+size);
            return *this;
        }
        pool& operator << (const array<str> & a)
        {
            *this << a.size(); for (const auto & s : a) *this << s;
            return *this;
        }
    };

    struct file
    {
        pool pool; std::ofstream fstream; path path; int32_t size = 0;

       ~file () { fstream.write((char*)pool.bytes.data(), pool.bytes.size()); }
        file (std::filesystem::path p, std::ios_base::openmode mode = std::ios::binary)
            : fstream (p, mode), path(p)
        {
            auto dir = path.parent_path();
            if (dir != std::filesystem::path())
                std::filesystem::create_directories(dir);
        }

        template <typename entry> 
        file & operator << (entry && e)
        {
            auto nn = pool.bytes.size();  pool << e;
            size += pool.bytes.size() - nn;
            if (pool.bytes.size() >= 8*1024*1024) {
                fstream.write((char*)pool.bytes.data(), pool.bytes.size());
                pool.bytes.clear();
            }
            return *this;
        }
    };
}
