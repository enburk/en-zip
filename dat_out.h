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
            bytes.insert(bytes.end(), data, data+size);
            return *this;
        }
        pool& operator << (const str & s)
        {
            *this << s.size();
            int size = s.size();
            byte* data = (byte*)(s.c_str()); 
            bytes.insert(bytes.end(), data, data+size);
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
}
