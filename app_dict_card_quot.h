#pragma once
#include "app_dict_html.h"
#include "app_dict_media.h"
#include "app_dict_card_quot_audio.h"
namespace app::dict::card
{
    struct quot : gui::widget<quot>
    {
        array<str> excluded_links;
        array<mediae::media_index> indices;
        gui::widgetarium<audio> players;
        gui::property<int> current = 0;
        gui::property<gui::time> timer;
        gui::button autoplay;
        gui::button play, Play;
        gui::button prev, next;
        gui::image speaker;
        gui::time smoothly {500};
        gui::text::view oneof;
        bool playall = false;

        quot ()
        {
            autoplay.text.text = "auto";
            autoplay.kind = gui::button::toggle;
            prev.text.text = (char*)(u8"\u00AB");
            next.text.text = (char*)(u8"\u00BB");
            prev.enabled = false;
            next.enabled = false;
            speaker.alpha = 64;
            oneof.hide();
            play.hide();
            Play.hide();
            on_change(&skin);
        }

        void reset_audio ()
        {
            indices = mediae::selected_audio;

            prev.enabled  = indices.size() > 1;
            next.enabled  = indices.size() > 1;
            speaker.alpha = indices.size() > 0 ? 128 : 64;
            oneof.show     (indices.size() > 0);
            play .show     (indices.size() > 0);
            Play .show     (indices.size() > 0);

            if (players.size() > 0)
            {
                auto it = indices.find(
                players(current).index);
                if (it != indices.end())
                {
                    players.rotate(0, current, current+1);
                    std::rotate(indices.begin(),
                        it, std::next(it));
                }
            }

            int n = 0;
            for (auto index : indices)
                players(n++).reset(index);

            playall = autoplay.on;
            players.truncate(n);
            current = 0;

            for (auto & player : players)
            {
                player.hide();
                player.coord = players.coord.now.local();
                player.text.excluded_links =
                player.text.excluded_links =
                    excluded_links;
            }

            using state = gui::media::state;

            if (players(current).state == state::ready   or
                players(current).state == state::playing or
                players(current).state == state::finished) 
                players(current).show();
        }

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go (gui::time::infinity,
                          gui::time::infinity);

            if (what == &coord)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height*12/7;
                int w = h*3/2;

                autoplay.coord = XYWH(0*w, 0*h, 2*w, h);
                play    .coord = XYWH(0*w, 1*h, 1*w, h);
                Play    .coord = XYWH(1*w, 1*h, 1*w, h);
                oneof   .coord = XYWH(0*w, 2*h, 2*w, h);
                prev    .coord = XYWH(0*w, 3*h, 1*w, h);
                next    .coord = XYWH(1*w, 3*h, 1*w, h);
                speaker .coord = XYWH(2*w, 0*h, 2*w, 2*w);
                players .coord = XYXY(5*w, 0, W, H);

                for (auto & player : players)
                    player.coord = players.
                        coord.now.local();
            }

            if (what == &timer)
            {
                if (players.size() > 0)
                {
                    using state = gui::media::state;
                    switch(players(current).state) {
                    case state::failure:
                        if (log) *log <<
                        "quot::failure " +
                         std::to_string(current);
                        break;
                    case state::ready:
                        if (autoplay.on)
                        players(current).play(); else
                        players(current).state = gui::media::state::finished;
                        players(current).show(smoothly);
                        break;
                    case state::finished:
                        if (players.size() < 2
                            or not playall)
                            break;
                        players(current).stop();
                        players(current).hide(smoothly);
                        current = (current+1) % players.size();
                        players(current).show(smoothly); if (current != 0)
                        players(current).play(); else playall = false;
                        break;
                    default:
                        break;
                    }

                    bool playing = players(current).state == state::playing;
                    play.text.text = (char*)(playing ? u8"\u23F8" : u8"\u25B6");
                    Play.text.text = (char*)(playing ? u8"\u23F9" : u8"\u23E9");

                    oneof.text =
                        std::to_string(current+1) + " of " +
                        std::to_string(players.size());
                }
            }

        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &players)
            {
                clicked = players.notifier->clicked;
                notify();
            }

            if (what == &autoplay)
            {
                playall = autoplay.on;
                if (playall and
                    players.size() > 0)
                    players(current).play();
            }
            if (what == &play) playall = false;
            if (what == &Play) playall = true;
            if (what == &play or
                what == &Play)
            {
                if (players(current).state == gui::media::state::playing) {
                    players(current).stop(); playall = false; } else
                    players(current).play();

                bool playing = players(current).state == gui::media::state::playing;
                play.text.text = (char*)(playing ? u8"\u23F8" : u8"\u25B6");
                Play.text.text = (char*)(playing ? u8"\u23F9" : u8"\u23E9");
            }

            int pp = players.size();

            if (what == &prev)
            {
                players(current).stop();
                players(current).hide(smoothly);
                current = (current-1+pp) % pp;
                players(current).show(smoothly);
                players(current).play();
            }
            if (what == &next)
            {
                players(current).stop();
                players(current).hide(smoothly);
                current = (current+1) % pp;
                players(current).show(smoothly);
                players(current).play();
            }
        }
    };
}

