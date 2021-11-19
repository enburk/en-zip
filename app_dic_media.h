#pragma once
#include "app.h"
namespace app::dic::media
{
    struct entry_index { int32_t entry, media; };
    struct media_index
    {
        str kind, title, comment, credit;
        ::media::location location;
        array<str> options;

        bool operator == (media_index const&) const = default;
        bool operator != (media_index const&) const = default;
    };
    array<entry_index> entry_indices;
    array<media_index> media_indices;

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

    str canonical (str s)
    {
        if (s.ends_with("}")) {
            str sense; s.split_by("{",
                s, sense); s.strip(); }

        auto ss = s.split_by("_"); if (ss.size() > 1)
        {
            s = ss.front(); ss.upto(1).erase();
            
            for (str ww : ss)
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

    str log (media_index const& index)
    {
        str title = doc::html::untagged(canonical(index.title));
        auto text = doc::text::text{title}; // do not brake UTF-8

        int nn = 0;
        title.clear();
        for (const auto& line : text.lines)
        {
            for (const str& glyph : line)
            {
                nn++;
                if (nn == 72) title += "..."; 
                if (nn >= 72) break;
                title += glyph;
            }
            title += " ";
        }

        str kind =
            index.kind == "audio" ? green ("[audio]"):
            index.kind == "video" ? purple("[video]"): "";
        title =
            index.kind == "audio" ? gray(title):
            index.kind == "video" ? dark(title): "";

        return "<a href=\"" +
        std::to_string(index.location.source) + ":" +
        std::to_string(index.location.offset) + "\">" +
        kind + " " + title + "</a>";
    }

    struct syncronizer
    {
        array<XY> pairs;

    };

    struct selected
    {
        array<media_index> audio;
        array<media_index> video;
        array<media_index> vudio;
        syncronizer syncronizer;
    };

    selected select (int n)
    {
        selected selected;
        auto& audios = selected.audio;
        auto& videos = selected.video;
        auto& vudios = selected.vudio;

        auto range = entry_indices.equal_range(
            entry_index{n, 0}, [](auto a, auto b)
                { return a.entry < b.entry; });

        for (auto [enty, media]: range) {
            auto& index = media_indices[media];
            if (index.kind == "audio") audios += index;
            if (index.kind == "video") videos += index;
        }

        for (int j=0; j<audios.size(); j++)
            logs::audio << log(audios[j]);

        auto uniquify = [](array<media_index>& a)
        {
            auto i = a.begin();
            auto j = a.begin() + 1;
            while (i != a.end())
            {
                while (j != a.end() and
                    j->title == i->title)
                    j++;

                if (int n = (int)(j - i); n > 1)
                {
                    int r = aux::random(0, n-1);
                    std::iter_swap(i, i+r); i++;
                    std::rotate(i, j, a.end());
                    a.resize(a.size() - (n-1));
                    j = i+1;
                }
                else { i++; j++; }
            }
        };

        uniquify(audios);
        uniquify(videos);
        vudios.resize(videos.size());

        for (int i=0; i<videos.size(); i++)
        for (int j=0; j<audios.size(); j++)
        {
            if (videos[i].title ==
                audios[j].title) {
                vudios[i] = audios[j];
                audios.erase(j);
                break;
            }
        }

        for (int i=0; i<videos.size(); i++) {
            logs::video << log(videos[i]);
            if (vudios[i] != media_index{})
            logs::video << log(vudios[i]);
        }

        return selected;
    }
}
