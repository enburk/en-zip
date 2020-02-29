#pragma once
#include "eng.h"
#include <fstream>
#include <filesystem>
namespace dat::in
{
    using std::byte;
    using std::filesystem::path;

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
                n = (static_cast<uint8_t>(bytes[offset+3]) & 0xFF)       |
                    (static_cast<uint8_t>(bytes[offset+2]) & 0xFF) <<  8 |
                    (static_cast<uint8_t>(bytes[offset+1]) & 0xFF) << 16 |
                    (static_cast<uint8_t>(bytes[offset  ]) & 0xFF) << 24;
            }
            else if (endianness == 0x78563412)
            {
                n = (static_cast<uint8_t>(bytes[offset  ]) & 0xFF)       |
                    (static_cast<uint8_t>(bytes[offset+1]) & 0xFF) <<  8 |
                    (static_cast<uint8_t>(bytes[offset+2]) & 0xFF) << 16 |
                    (static_cast<uint8_t>(bytes[offset+3]) & 0xFF) << 24;
            }
            else throw std::logic_error ("dat::in::pool: unsupported endianness:" +
                std::to_string (endianness));

            offset += 4;
            return n;
        }

        std::string_view get_string ()
        {
            int size = get_int();

            if (size < 0) throw
                std::out_of_range("dat::in::pool " + name + ": get_string negative size");
            if (offset + size > bytes.size()) throw
                std::out_of_range("dat::in::pool " + name + ": get_string out of range");
        
            std::string_view s {(char*)(&bytes[offset]), (size_t)(size)};
            offset += size;
            return s;
        }

        std::pair<byte*, int> get_bytes ()
        {
            int size = get_int();
                
            if (size < 0) throw
                std::out_of_range("dat::in::pool " + name + ": get_bytes negative size");
            if (offset + size > bytes.size()) throw
                std::out_of_range("dat::in::pool " + name + ": get_bytes out of range");
        
            std::pair<byte*, int> data {&bytes[offset], size};
            offset += size;
            return data;
        }
    };
}
