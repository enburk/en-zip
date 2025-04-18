#pragma once
#include "abc.h"
namespace media
{
    using sys::byte;
    using aux::expected;
    using std::filesystem::path;

    array<str> videoexts = {".gif", ".png", ".jpg", ".jpeg" };
    array<str> audioexts = {".mp3", ".ogg", ".wav"};

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

    using index = media_index;

    array<byte> bytes (index i)
    {
        if (i == index{}) return {};

        location l = i.location;

        path source = "../data/media/storage." +
            std::to_string(l.source) + ".dat";

        return sys::bytes(source,
            l.offset,
            l.length);
    }
    array<array<byte>> bytes (array<index> ii) {
    array<array<byte>> bytess; bytess.reserve(ii.size());
      for (auto i: ii) bytess.emplace_back(bytes(i));
      return bytess; } 

    str canonical (str s)
    {
        array<str> ss = s.split_by("_"); if (ss.size() > 1)
        {
            s = ss.front(); ss.upto(1).erase();
            
            for (str x: ss)
            {
                int n = 0;
                for (char c : x)
                    if((c < '0') || ('9' < c &&
                        c < 'A') || ('Z' < c &&
                        c < 'a') || ('z' < c))
                        break; else n++;

                str word = x.upto(n);
                str rest = x.from(n);

                s += linked(word,word) + rest;
            }
        }

        s.replace_all("---", mdash);
        s.replace_all("--" , ndash);

        return s;
    }

    str log (index const& index)
    {
        str s = doc::html::untagged(
            canonical(index.title));

        if (index.options.
            contains("sound"))
            s = "[" + s + "]";

        str kind =
            index.kind == "audio" ? green ("[audio]"):
            index.kind == "video" ? purple("[video]"):
            "";
        str title =
            index.kind == "audio" ? gray(s):
            index.kind == "video" ? dark(s):
            "";

        return linked(
        kind + " " + title,
        "file://" + index.path);
    }
    array<str> log (array<index> const& index) {
    array<str> logs; for (auto& i: index)
        logs += log(i);
        return logs; }

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
    namespace info
    {
        hashmap<str,str> shortenings;
    }
}