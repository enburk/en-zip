#pragma once
#include "app.h"
namespace app::dict::mediae
{
    struct entry_index { int32_t entry, media; };
    struct media_index
    {
        str kind, title, comment, credit;
        media::data::location location;
        array<str> options;

        bool operator == (media_index const&) const = default;
        bool operator != (media_index const&) const = default;
    };
    array<entry_index> entry_indices;
    array<media_index> media_indices;
    array<media_index> selected_audio;
    array<media_index> selected_video;

    void reload ()
    {
        dat::in::pool pool;
        std::filesystem::path dir = "../data/app_dict";
        if (!std::filesystem::exists (dir / "entry_index.dat")) return;
        pool.bytes = dat::in::bytes(dir / "entry_index.dat").value();
        media_indices.resize(pool.get_int());
        entry_indices.resize(pool.get_int());
        for (auto & index : entry_indices) {
            index.entry = pool.get_int();
            index.media = pool.get_int();
        }
        pool.offset = 0; // reuse
        pool.bytes = dat::in::bytes(dir / "media_index.dat").value();
        for (auto & index : media_indices) {
            index.kind    = pool.get_string();
            index.title   = pool.get_string();
            index.comment = pool.get_string();
            index.credit  = pool.get_string();
            index.options.resize(pool.get_int());
            for (auto & option : index.options)
                option = pool.get_string();
            pool >> index.location;
        }
    }

    void select (int n)
    {
        selected_audio.clear();
        selected_video.clear();

        auto range = entry_indices.equal_range(entry_index{n, 0},
            [](auto a, auto b){ return a.entry < b.entry; });

        for (auto [enty, media] : range)
        {
            auto & index = media_indices[media];

            if (index.kind == "audio") selected_audio += index;
            if (index.kind == "video") selected_video += index;
        }
    }

    str canonical (str s)
    {
        if (s.ends_with("}")) {
            str sense; s.split_by("{",
                s, sense); s.strip(); }

        auto ss = s.split_by("_"); if (ss.size() > 1)
        {
            s = ss.front(); ss.upto(1).erase(); for (str ww : ss)
            {
                int n = 0;
                for (char c : ww)
                    if((c < '0') || ('9' < c &&
                        c < 'A') || ('Z' < c &&
                        c < 'a') || ('z' < c))
                        break; else n++;

                str w = ww.upto(n);
                str r = ww.from(n);

                s += "<a href=\"" + w + "\">"
                    + w + "</a>" + r;
            }
        }

        return s;
    }
}
