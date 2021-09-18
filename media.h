#pragma once
#include "abc.h"
#include "data_in.h"
#include "data_out.h"
namespace media
{
    using std::filesystem::path;
    using aux::expected;
    using sys::byte;

    struct resource
    {
        path path;
        str id, kind;
        str title, comment, credit;
        array<str> options;
        array<str> entries;
        array<str> keywords;
    };

    struct location
    {
        int32_t source = 0;
        int32_t offset = 0;
        int32_t length = 0;
        int32_t size_x = 0;
        int32_t size_y = 0;

        bool operator == (location const&) const = default;
        bool operator != (location const&) const = default;

        friend void operator >> (dat::in::pool & in, location & l) {
            l.source = in.get_int();
            l.offset = in.get_int();
            l.length = in.get_int();
            l.size_x = in.get_int();
            l.size_y = in.get_int();
        }
        friend void operator << (dat::out::pool & out, location & l) {
            out << l.source;
            out << l.offset;
            out << l.length;
            out << l.size_x;
            out << l.size_y;
        }
    };

    namespace report
    {
        gui::console * out = nullptr;
        gui::console * err = nullptr;
        std::unordered_map<str, array<path>> id2path;
        array<path> unidentified;
        bool data_updated = false;
    }

}