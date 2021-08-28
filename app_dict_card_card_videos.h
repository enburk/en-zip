#pragma once
#include "app_dict_html.h"
#include "app_dict_media.h"
#include "app_dict_card_card_video.h"
namespace app::dict::video
{
    struct sequencer : gui::widget<sequencer>
    {
        gui::widgetarium<player> players;
        gui::property<int> current = 0;
        gui::property<gui::time> timer;
        array<str> excluded_links;
        gui::time smoothly {1000};

        void reset ()
        {
            auto indices = mediae::selected_video;

            if (players.size() > 0)
            {
                auto it = std::ranges::find(
                indices, players(current).index);
                if (it != indices.end())
                {
                    players.rotate(0, current, current+1);
                    std::rotate(indices.begin(),
                        it, std::next(it));
                }
            }

            int n = 0;
            for (auto index : indices)
                players(n++).reset(index, excluded_links);

            players.truncate(n);
            current = 0;

            for (auto & player : players)
            {
                player.hide();
                player.coord = coord.now.local();
                player.prev.enabled = n > 1;
                player.next.enabled = n > 1;
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

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go (gui::time::infinity,
                          gui::time::infinity);

            if (what == &coord && coord.was.size != coord.now.size)
            {
                players.coord = coord.now.local();
                for (auto & player : players)
                    player.coord = coord.now.local();
            }

            if (what == &timer)
            {
                if (players.size() > 0)
                {
                    using state = gui::media::state;
                    switch(players(current).state) {
                    case state::failure:
                        if (log) *log << "quot::failure " +
                        std::to_string(current) + ": " +
                        players(current).error;
                        players(current).state = gui::media::state::finished;
                        players(current).show(smoothly);
                        break;
                    case state::ready:
                        players(current).play();
                        players(current).show(smoothly.ms/2);
                        break;
                    case state::finished:
                        if (players.size() == 1) break;
                        players(current).stop();
                        players(current).hide(smoothly);
                        current = (current+1) % players.size();
                        players(current).show(smoothly);
                        players(current).play();
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            clicked = players.notifier->clicked;

            if (clicked == -2)
            {
                XY p =
                sys::mouse::position() -
                players(current).prev.coord.now.origin;

                players(current).stop();
                players(current).hide(smoothly.ms/2);
                current = (current-1+players.size()) % players.size();
                players(current).show(smoothly.ms/2);
                players(current).play();

                sys::mouse::position(p +
                players(current).prev.coord.now.origin);
                return;
            }
            if (clicked == -1)
            {
                XY p =
                sys::mouse::position() -
                players(current).next.coord.now.origin;

                players(current).stop();
                players(current).hide(smoothly.ms/2);
                current = (current+1) % players.size();
                players(current).show(smoothly.ms/2);
                players(current).play();

                sys::mouse::position(p +
                players(current).next.coord.now.origin);
                return;
            }
            
            notify();
        }
    };
}