#pragma once
#include "abc.h"
#include <fstream>
#include <filesystem>
namespace dat::out
{
    using std::byte;
    using std::filesystem::path;

    void write (path path, array<byte> const& data)
    {
        std::filesystem::create_directories(path.parent_path());
        std::ofstream ofstream(path, std::ios::binary);
        ofstream.write((char*)(data.data()), data.size());
    }

    void write (path path, array<str> const& text)
    {
        std::filesystem::create_directories(path.parent_path());
        std::ofstream ofstream(path);
        for (str line: text) {
            ofstream << line;
            ofstream << "\n";
        }
    }

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
            bytes.container::insert(bytes.end(), data, data+size);
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
        pool pool;
        path path;
        std::ofstream fstream;
        int size = 0;

        file (std::filesystem::path path,
            std::ios_base::openmode mode = std::ios::binary)
            : path(path)
        {
            auto dir = path.parent_path();
            if  (dir != std::filesystem::path())
                std::filesystem::create_directories(dir);

            fstream = std::ofstream(path, mode);

            if ((mode & std::ios::app) != 0 and
                std::filesystem::exists(path))
                size = (int)std::filesystem::
                    directory_entry(path)
                        .file_size();
        }

        ~file () { close(); }

        void close ()
        {
            fstream.write((char*)
                pool.bytes.data(),
                pool.bytes.size());
            fstream.close();
        }

        template <typename entry> 
        file & operator << (entry && e)
        {
            auto nn = pool.bytes.size();  pool << e;
            size += pool.bytes.size() - nn;
            if (pool.bytes.size() >= 8*1024*1024) {
                fstream.write((char*)
                pool.bytes.data(),
                pool.bytes.size());
                pool.bytes.clear();
            }
            return *this;
        }
    };
}
