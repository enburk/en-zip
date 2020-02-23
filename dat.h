#pragma once
#include "eng.h"
#include <fstream>
#include <filesystem>
#include <zlib.h>
namespace dat
{
    using std::byte;
    using std::filesystem::path;

    namespace in
    {
        inline int32_t endianness = 0; // 0 is important for the first get int

        struct pool
        {
            str name;
            int offset = 0;
            array<byte> bytes;

            int get_int ()
            {
                if (offset + 4 > bytes.size()) throw
                    std::out_of_range("dat::in::pool " + name + ": get_int out of range");

                int n = 0;

                if (endianness == 0x12345678 || endianness == 0)
                {
                    n = (std::to_integer<int>(bytes [offset+3]) & 0xFF)       |
                        (std::to_integer<int>(bytes [offset+2]) & 0xFF) <<  8 |
                        (std::to_integer<int>(bytes [offset+1]) & 0xFF) << 16 |
                        (std::to_integer<int>(bytes [offset  ]) & 0xFF) << 24;
                }
                else if (endianness == 0x78563412)
                {
                    n = (std::to_integer<int>(bytes [offset  ]) & 0xFF)       |
                        (std::to_integer<int>(bytes [offset+1]) & 0xFF) <<  8 |
                        (std::to_integer<int>(bytes [offset+2]) & 0xFF) << 16 |
                        (std::to_integer<int>(bytes [offset+3]) & 0xFF) << 24;
                }
                else throw std::logic_error ("dat::in::pool: unsupported endianness:" +
                    std::to_string (endianness));

                offset += 4;
                return n;
            }

            std::string_view get_string ()
            {
                int size = get_int();

                if (offset + size > bytes.size()) throw
                    std::out_of_range("dat::in::pool " + name + ": get_string out of range");
        
                std::string_view s {(char*)(&bytes[offset]), (size_t)(size)};
                offset += size;
                return s;
            }

            std::pair<byte*, int> get_bytes ()
            {
                int size = get_int();
                
                if (offset + size > bytes.size()) throw
                    std::out_of_range("dat::in::pool " + name + ": get_bytes out of range");
        
                std::pair<byte*, int> data {&bytes[offset], size};
                offset += size;
                return data;
            }
        };
    }

    namespace out
    {
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
                int size = s.size() + 1;
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

        struct file
        {
            path path; int64_t size;
        };
    }

    array<byte> zip (array<byte> bytes)
    {
    }

    array<byte> unzip (array<byte> bytes, int uncompressed_size)
    {
        //uLongf unzsize = 
        //uncompress ( (Bytef*) dst, &uncompressed_size, (Bytef*) src, srcsize );

    }
}
