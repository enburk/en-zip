#pragma once
#include "app.h"
namespace app::dic::media
{
    using ::media::index;
    using ::media::canonical;
    using ::media::log;

    struct syncronizer
    {
        array<xy> pairs;
    };

    struct selected
    {
        array<index> audio;
        array<index> video;
        array<index> vudio;
        syncronizer syncronizer;
    };

    selected select (int n)
    {
        selected selected;
        auto& audios = selected.audio;
        auto& videos = selected.video;
        auto& vudios = selected.vudio;

        auto range = mediadata.entries_dic.equal_range(
        ::media::entry_index{n, 0}, [](auto a, auto b)
        { return a.entry < b.entry; });

        for (auto [entry, media]: range) {
        auto& index = mediadata.media_index[media];
        if (index.kind == "audio") audios += index;
        if (index.kind == "video") videos += index; }

        for (int j=0; j<audios.size(); j++)
            logs::audio << log(audios[j]);

        auto uniquify = [](array<index>& a)
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
            if (vudios[i] != index{})
            logs::video << log(vudios[i]);
        }

        return selected;
    }
}
