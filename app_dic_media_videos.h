#pragma once
#include "app_dic_media_video.h"
namespace app::dic::video
{
    struct sequencer:
    widget<sequencer>
    {
        gui::widgetarium<player> players;
        gui::property<int> current = 0;
        gui::property<bool> mute = false;
        gui::property<gui::time> timer;
        gui::time smoothly {1000};
        int clicked = 0;

        void reset (str title,
            array<media::media_index> videos,
            array<media::media_index> audios,
            array<str> const& links)
        {
            if (players.size() > 0)
            {
                auto it = std::find(
                videos.begin(), videos.end(), players(current).index);
                if (it != videos.end())
                {
                    players.rotate(0, current, current+1);
                    std::rotate(videos.begin(), it, std::next(it));
                    it = audios.begin() + (it - videos.begin());
                    std::rotate(audios.begin(), it, std::next(it));
                }
            }

            int n = videos.size();
            for (int i=0; i<n; i++)
            players(i).reset(title, videos[i], audios[i], links);
            players.truncate(n);
            current = 0;

            for (auto & player : players)
            {
                player.hide();
                player.coord = coord.now.local();
                player.prev.enabled = n > 1;
                player.next.enabled = n > 1;
                player.mute = mute.now;
            }

            using state = gui::media::state;

            if (players.size() > 0)
            if (players(0).state == state::ready   or
                players(0).state == state::playing or
                players(0).state == state::finished) {
                players(0).show();
                players(0).play();
            }
        }

        int height (int width)
        {
            int height = 0;
            for (auto & player : players)
            height = max (height, player.height(width));
            return height;
        }

        void on_change(void* what) override
        {
            if (timer.now == gui::time())
                timer.go(gui::time::infinity,
                    gui::time::infinity);

            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                for (auto& player: players)
                player .coord = coord.now.local();
                players.coord = coord.now.local();
            }

            if (what == &mute)
            {
                for (auto& player : players)
                player.mute = mute.now;
            }

            if (what == &timer)
            {
                if (players.size() > 0)
                {
                    using state = gui::media::state;
                    switch (players(current).state) {
                    case state::failure:
                        logs::times << "video: " +
                        std::to_string(current) + ": " +
                        players(current).error;
                        players(current).state = gui::media::state::finished;
                        players(current).show(smoothly);
                        break;
                    case state::ready:
                        players(current).play();
                        players(current).show(smoothly.ms / 2);
                        break;
                    case state::finished:
                        if (players.size() == 1) break;
                        players(current).stop();
                        players(current).hide(smoothly);
                        current = (current + 1) % players.size();
                        players(current).show(smoothly);
                        players(current).play();
                        break;
                    default:
                        break;
                    }
                }
            }
            if (what == &players)
            {
                clicked = players.notifier->clicked;

                if (clicked == -2)
                {
                    xy p =
                        sys::mouse::position() -
                        players(current).prev.coord.now.origin;

                    players(current).stop();
                    players(current).hide(smoothly.ms / 2);
                    current = (current - 1 + players.size()) % players.size();
                    players(current).show(smoothly.ms / 2);
                    players(current).play();

                    sys::mouse::position(p +
                        players(current).prev.coord.now.origin);
                    return;
                }
                if (clicked == -1)
                {
                    xy p =
                        sys::mouse::position() -
                        players(current).next.coord.now.origin;

                    players(current).stop();
                    players(current).hide(smoothly.ms / 2);
                    current = (current + 1) % players.size();
                    players(current).show(smoothly.ms / 2);
                    players(current).play();

                    sys::mouse::position(p +
                        players(current).next.coord.now.origin);
                    return;
                }

                notify();
            }
        }
    };
}