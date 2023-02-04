#pragma once
#include "app.h"
namespace app::dic::media
{
    using ::media::in::entry_index;
    using ::media::in::media_index;

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
        array<xy> pairs;

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

        auto range = mediadata.entries_dic.equal_range(
            entry_index{n, 0}, [](auto a, auto b)
                { return a.entry < b.entry; });

        for (auto [enty, media]: range) {
        auto& index = mediadata.media_index[media];
        if (index.kind == "audio") audios += index;
        if (index.kind == "video") videos += index; }

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

    //  uniquify(audios);
    //  uniquify(videos);
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
