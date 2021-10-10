#pragma once
#include "app_dic_html.h"
#include "app_dic_media.h"
#include "app_dic_media_audio.h"
namespace app::dic::left
{
    struct quot:
    widget<quot>
    {
        gui::button autoplay;
        gui::button play, Play;
        gui::button stop, Stop;
        gui::button prev, next;
        gui::player speaker;
        gui::text::view oneof;

        gui::widgetarium<audio::player> players;
        gui::property<int> current = 0;
        gui::property<gui::time> timer;
        gui::time smoothly {500};
        bool playall = false;

        std::thread thread;
        std::atomic<bool> cancel = false;
        std::mutex mutex;

        quot ()
        {
            autoplay.text.text = "auto";
            autoplay.kind = gui::button::toggle;
            prev.enabled = false;
            next.enabled = false;
            speaker.alpha = 64;
            oneof.hide();
            play.hide();
            Play.hide();
            stop.hide();
            Stop.hide();
            on_change(&skin);
        }
        ~quot ()
        {
            cancel = true;
            if (thread.joinable())
                thread.join();
        }

        void reload ()
        {
            prev.icon.load(assets["icon.chevron.left.double.black.128x128"]);
            next.icon.load(assets["icon.chevron.right.double.black.128x128"]);
            play.icon.load(assets["player.black.play.64x64"]);
            Play.icon.load(assets["player.black.next.64x64"]);
            stop.icon.load(assets["player.black.pause.64x64"]);
            Stop.icon.load(assets["player.black.stop.64x64"]);
        }

        using idx = media::media_index;

        void reset (array<idx> selected, array<str> const& links)
        {
            cancel = true;

            prev.enabled  = selected.size() > 1;
            next.enabled  = selected.size() > 1;
            speaker.alpha = selected.size() > 0 ? 128 : 64;
            oneof.show     (selected.size() > 0);

            if (players.size() > 0)
            {
                auto it = std::ranges::find(
                selected, players(current).index);
                if (it != selected.end())
                {
                    players.rotate(0, current, current+1);
                    std::rotate(selected.begin(),
                        it, std::next(it));
                }
            }

            int n = 0;
            for (auto index : selected) {
                players(n++).reset(index, links);
            }

            playall = autoplay.on;
            players.truncate(n);
            current = 0;

            for (auto & player : players)
            {
                player.hide();
                player.coord = players.coord.now.local();
            }

            using state = gui::media::state;

            if (players(current).state == state::ready   or
                players(current).state == state::playing or
                players(current).state == state::finished) 
                players(current).show();

            if (thread.joinable())
                thread.join();

            cancel = false;

            thread = std::thread([this]()
            {
                for (auto & player : players) {
                    player.load(mutex, cancel);
                    if (cancel) break;
                }
            });
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

                int l = h*27/100;
                int L = h*39/100;
                play.icon.padding = XYXY(L,l,L,l);
                stop.icon.padding = XYXY(L,l,L,l);
                Stop.icon.padding = XYXY(L,l,L,l);
                l = h*10/100;
                L = h*18/100;
                Play.icon.padding = XYXY(L,l,L,l);
                l = h*30/100;
                prev.icon.padding = XYXY(l,l,l,l);
                next.icon.padding = XYXY(l,l,l,l);

                int w = h*5/4;
                int d = h*1/2;
                int y = 2*w*96/128;

                speaker .coord = XYWH(0*w, 0+0*h, 2*w, y);
                autoplay.coord = XYWH(0*w, y+0*h, 2*w, h);
                play    .coord = XYWH(0*w, y+1*h, 1*w, h);
                stop    .coord = XYWH(0*w, y+1*h, 1*w, h);
                Play    .coord = XYWH(1*w, y+1*h, 1*w, h);
                Stop    .coord = XYWH(1*w, y+1*h, 1*w, h);
                oneof   .coord = XYWH(0*w, y+2*h, 2*w, h);
                prev    .coord = XYWH(0*w, y+3*h, 1*w, h);
                next    .coord = XYWH(1*w, y+3*h, 1*w, h);
                players .coord = XYXY(2*w+d, 0, W-d-d, H);

                if (speaker.state == gui::media::state::vacant)
                    speaker.load(assets["speaker.128x096"]
                    .from(0));

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
                        players(current).state = gui::media::state::finished;
                        players(current).show(smoothly);
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
                    play.hide(playing);
                    Play.hide(playing);
                    stop.show(playing);
                    Stop.show(playing);

                    oneof.text =
                        std::to_string(current+1) + " of " +
                        std::to_string(players.size());
                }
                else
                {
                    play.hide();
                    Play.hide();
                    stop.hide();
                    Stop.hide();
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
            if (what == &play or what == &stop or
                what == &Play or what == &Stop)
            {
                if (players(current).state == gui::media::state::playing) {
                    players(current).stop(); playall = false; } else
                    players(current).play();
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

