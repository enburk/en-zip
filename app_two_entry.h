#pragma once
#include "app.h"
#include "app_two_video.h"
namespace app::two
{
    struct entry:
    widget<entry>
    {
        sfx::media::
        sequencer<video::player> player;
        text::cell script;

        property<int>  number = -1;
        property<bool> translated = false;
        property<byte> volume = 255;
        property<bool> mute = false;

        content::unit* topic = nullptr;
        bool pixed = false;
        int seconds = 0;
        int clicked = 0;
        str text;

        void init ()
        {
            player.players.clear();

            if (number < 0)
                return;

            array<media::index> audios;
            array<media::index> videos;

            auto range = mediadata.entries_two.
            equal_range(media::entry_index{number, 0},
                [](auto a, auto b){ return
                a.entry < b.entry; });

            for (auto [entry, media]: range)
            {
                auto& i = mediadata.media_index[media];
                if (i.kind == "video") videos += i;
                if (i.kind == "audio") audios += i;
            }

            pixed = not videos.empty();

            audios.erase_if([](auto& audio){
            return audio.options.contains("xlam"); });

            aux::shuffle(videos.begin(), videos.end());
            aux::shuffle(audios.begin(), audios.end());

            auto const& entry =
            course.entries[number];

            for (auto& video: videos)
            if (not video.options.contains("texted"))
            {
                str t = video.title;
                str s = eng::lowercased(media::canonical(t));
                str E = eng::lowercased(entry.eng);
                E.replace_all("\\\\", "/");
                E.replace_all("'''", "");
                E.replace_all("''", "");
                E.replace_all("~", "");
                E.replace_all("[", "");
                E.replace_all("]", "");
                E.canonicalize();
                if (E.contains(s))
                video.options +=
                "texted";
            }

            for (int i=0; i<videos.size(); i++)
                player.players[i].load(videos[i],
                    audios.empty() ? media::index{} :
                    audios[i % audios.size()]);

            player.Play();
        }

        void translate ()
        {
            if (number < 0)
                return;

            auto const& entry =
            course.entries[number];

            script.html = entry.html(translated);
        }

        xy resize (int w, int h)
        {
            timing t0;

            int l = gui::metrics::line::width;
            int d = gui::metrics::text::height;
            int W = w;

            if (w < l+l) return xy{}; w -= l+l; 
            if (h < l+l) return xy{}; h -= l+l; 

            script.alignment = xy{pix::center, pix::top};
            script.coord = xywh(l, l, w, h);
            script.resize(script.text_size());

            player.fit(xy{w, h - script.coord.now.h});

            xy psize = player.coord.now.size;
            xy ssize = script.coord.now.size;

            player.move_to(xy(l + w/2 - psize.x/2, l));
            script.move_to(xy(l + w/2 - ssize.x/2, l + psize.y));

            return xy{W, l + psize.y + ssize.y + l};
        }

        xy resize_to_fit (int w, int h)
        {
            xy size = resize(w, h);

            return size;
        }

        void on_change (void* what) override
        {
            if (what == &number)
            {
                init();
                translate();
            }
            if (what == &translated)
            {
                translate();
            }

            if (what == &script) clicked = script.clicked, notify();
            if (what == &player) clicked = player.clicked, notify();
        }
    };
}

