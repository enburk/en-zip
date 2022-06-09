#pragma once
#include "app_dic_media_audio.h"
namespace app::dic::media
{
    struct player:
    widget<player>
    {
        gui::player video;
        audio::player audio;
        sys::thread thread;
        gui::property<bool> mute = false;

        void load (media_index video_index, media_index audio_index)
        {
            thread.stop = true;

            std::filesystem::path dir = "../data/app_dict";
            std::string storage = "storage." + std::to_string(
                video_index.location.source) + ".dat";

            video.load(dir / storage,
                video_index.location.offset,
                video_index.location.length);

            thread.join();

            audio.reset(audio_index, array<str>{});
            audio.muted = false;

            if (audio_index == media_index{}) {
                audio.state = gui::media::state::vacant;
                return; }

            thread = [this](auto& cancel)
            {
                audio.load(cancel);
            };
        }

        void play ()
        {
            video.play();
            audio.play();
        }

        void stop ()
        {
            video.stop();
            audio.stop();
        }

        gui::media::state state ()
        {
            auto v = video.state.load();
            auto a = audio.state.load();
            using s = gui::media::state;

            if (v == s::ready) return
                a == s::loading ?
                     s::loading :
                     s::ready;

            if (v == s::finished) return
                a == s::playing ?
                     s::playing :
                     s::finished;

            return v;
        }

        str error ()
        {
            return video.error;
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                video.coord = coord.now.local();
            }

            if (what == &mute)
            {
                audio.mute(mute.now);
            }
        }
    };
}
