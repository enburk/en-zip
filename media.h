#pragma once
#include "abc.h"
namespace media
{
    using std::filesystem::path;
    using aux::expected;
    using sys::byte;

    struct resource
    {
        path path;
        str id, kind;
        str title, sense, comment, credit;
        array<str> options;
        array<str> entries;
        array<str> keywords;
        int usage =  0;
        int index = -1;
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

        friend void operator >> (sys::in::pool& pool, location& l) {
            pool >> l.source;
            pool >> l.offset;
            pool >> l.length;
            pool >> l.size_x;
            pool >> l.size_y;
        }
        friend void operator << (sys::out::pool& pool, location const& l) {
            pool << l.source;
            pool << l.offset;
            pool << l.length;
            pool << l.size_x;
            pool << l.size_y;
        }
    };

    struct Location
    {
        location location;
        bool new_one = false;
    };

    struct entry_index
    {
        int32_t entry, media;

        bool operator == (entry_index const&) const = default;
        bool operator != (entry_index const&) const = default;

        friend void operator >> (sys::in::pool& pool, entry_index& i) {
            pool >> i.entry;
            pool >> i.media;
        }
        friend void operator << (sys::out::pool& pool, entry_index const& i) {
            pool << i.entry;
            pool << i.media;
        }
    };
    struct media_index
    {
        str kind;
        str title, sense;
        str comment, credit;
        location location;
        array<str> options;
        str path; // studio only

        bool operator == (media_index const&) const = default;
        bool operator != (media_index const&) const = default;

        friend void operator >> (sys::in::pool& pool, media_index& m) {
            pool >> m.kind;
            pool >> m.title;
            pool >> m.sense;
            pool >> m.comment;
            pool >> m.credit;
            pool >> m.options;
            pool >> m.location;
        }
        friend void operator << (sys::out::pool& pool, media_index const& m) {
            pool << m.kind;
            pool << m.title;
            pool << m.sense;
            pool << m.comment;
            pool << m.credit;
            pool << m.options;
            pool << m.location;
        }
    };

    namespace report
    {
        bool updated = false;
        array<path> unidentified;
        std::unordered_map<str, array<path>> id2path;
    }
    namespace logs
    {
        optional_log out;
        optional_log err;
    }
}