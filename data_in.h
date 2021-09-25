#pragma once
#include "abc.h"
#include <fstream>
#include <filesystem>
namespace dat::in
{
    using sys::byte;
    using std::filesystem::path;
    using aux::expected;

    expected<array<byte>> bytes (path path) try
    {
        std::ifstream ifstream (path, std::ios::binary);

        ifstream.seekg(0, std::ios::end);
        int size = (int)ifstream.tellg();
        ifstream.seekg(0, std::ios::beg);

        array<byte> pool;
        pool.resize(size);
        ifstream.read((char*)(pool.data()), size);

        return pool;
    }
    catch (std::exception & e) {
    return aux::error(e.what()); }

    expected<array<str>> text (path path) try
    {
        std::ifstream stream(path); str text = std::string{(
        std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()};

        if (text.starts_with("\xEF" "\xBB" "\xBF"))
            text.upto(3).erase(); // UTF-8 BOM

        return text.split_by("\n");
    }
    catch (std::exception & e) {
    return aux::error(e.what()); }

    inline int32_t endianness = 0;

    struct pool
    {
        str name;
        int offset = 0;
        array<byte> bytes;

        pool () = default;
        pool (path path) {
            name = path.string();
            if (!std::filesystem::exists(path)) return;
            bytes = dat::in::bytes(path).value();
        }

        void get_endianness () {
            endianness = 0; // or it would be reversed
            endianness = get_int();
        }

        int get_int ()
        {
            if (offset + 4 > bytes.size()) throw
                std::out_of_range("dat::in::pool " +
                    name + ": get_int out of range");

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
            else throw std::logic_error (
               "dat::in::pool: unsupported endianness:" +
                std::to_string(endianness));

            offset += 4;
            return n;
        }

        std::string_view get_string ()
        {
            int size = get_int();

            if (size < 0) throw
                std::out_of_range("dat::in::pool " + name +
                    ": get_string negative size");

            if (offset + size > bytes.size()) throw
                std::out_of_range("dat::in::pool " + name +
                    ": get_string out of range");
        
            std::string_view s {(char*)(&bytes[offset]), (size_t)(size)};
            offset += size;
            return s;
        }

        struct chunk { byte* data; int size; };

        chunk get_bytes ()
        {
            int size = get_int();
                
            if (size < 0) throw
                std::out_of_range("dat::in::pool " + name +
                ": get_bytes negative size");

            if (offset + size > bytes.size()) throw
                std::out_of_range("dat::in::pool " + name +
                    ": get_bytes out of range");
        
            chunk chunk{&bytes[offset], size};
            offset += size;
            return chunk;
        }
    };
}
