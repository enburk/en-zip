#pragma once
#include "app_dic_media.h"
#include "app_dic_media_video.h"
namespace app::dic::left
{
    struct card:
    widget<card>
    {
        html_view text;
        
        sfx::media::
        sequencer<video::player> video;
        gui::property<byte> volume = 255;
        gui::property<bool> mute = false;
        using idx = media::index;
        xy video_max_size;
        int clicked = 0;

        void reload () {}

        void reset (
            array<idx> videos,
            array<idx> audios,
            array<str> links)
        {
            if (
            videos.size() !=
            audios.size())
            throw std::logic_error(
            "should be same");

            text.forbidden_links = links;

            auto& players = video.players;
            auto& current = video.current;

            if (players.size() > 0)
            {
                auto it = std::ranges::find(
                videos, players[current].index);
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
            players[i].load(videos[i], audios[i], links);
            players.truncate(n);
            current = 0;

            for (auto& player: players)
            {
                //player.hide();
                player.coord = coord.now.local();
                player.prev.enabled = n > 1;
                player.next.enabled = n > 1;
                player.volume = volume.now;
                player.mute = mute.now;
            }

            using state = sfx::media::state;

            if (players.size() > 0)
            if (players[0].status == state::ready   or
                players[0].status == state::playing or
                players[0].status == state::finished)
                players[0].show();

            int l = gui::metrics::line::width;

            video_max_size = xy{};
            for (auto& v: videos)
            video_max_size.x = max(
            video_max_size.x, v.location.size_x + 6*l);
            video_max_size.y = video_height(
            video_max_size.x);

            video_resize(video_max_size);
            video.repeat = true;
            video.Play();
        }

        int video_height (int width)
        {
            int height = 0;
            for (auto& player: video.players)
            height = max (height, player.height(width));
            return height;
        }

        void video_resize (xy size)
        {
            int maxwidth = coord.now.size.x * 2/3;
            if (maxwidth < size.x) size = xy (
                maxwidth, video_height(
                maxwidth));

            video.resize(size);
            video_position();
        }

        void video_position ()
        {
            text.rwrap = array<xy>{
            xy{0, text.scroll.y.top},
            video.coord.now.size + xy{
            gui::metrics::line::width*3,
            gui::metrics::line::width*3}};

            int d = text.scroll.y.alpha.to == 0 ?
                0 : text.scroll.y.coord.now.w;

            video.move_to(xy(
            coord.now.size.x -
            video.coord.now.size.x -
            d, 0));
        }

        bool text_selected () { return text.view.selected() != ""; }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                text.coord = coord.now.local();
                video_resize(video.coord.now.size);
            }
            if (what == &text.scroll.y
            or  what == &text.update_text)
            {
                video_position();
            }
            if (what == &text ) { clicked = text .clicked; notify(); }
            if (what == &video) { clicked = video.clicked;

                if (clicked == -2)
                {
                    xy p =
                    sys::mouse::position() -
                    video.players[video.current].
                    prev.coord.now.origin;

                    video.prev();

                    sys::mouse::position(p +
                    video.players[video.current].
                    prev.coord.now.origin);
                }
                else
                if (clicked == -1)
                {
                    xy p =
                    sys::mouse::position() -
                    video.players[video.current].
                    next.coord.now.origin;

                    video.next();

                    sys::mouse::position(p +
                    video.players[video.current].
                    next.coord.now.origin);
                }
                else
                notify();
            }

            if (what == &volume)
                video.volume =
                volume;

            if (what == &mute)
                video.mute =
                mute;
        }
    };
}

