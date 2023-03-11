#pragma once
#include "app_dic_html.h"
#include "app_dic_media.h"
#include "app_dic_media_audio.h"
namespace app::dic::left
{
    struct quot:
    widget<quot>
    {
        gui::button mute;
        gui::button play, Play;
        gui::button stop, Stop;
        gui::button prev, next;
        sfx::image::player speaker;
        gui::text::view oneof;

        gui::widgetarium<audio::player> players;
        gui::property<int> current = 0;
        gui::property<gui::time> timer;
        gui::time smoothly {500};
        gui::time instantly{100};
        bool playall = false;
        int ready_players = 0;
        int clicked = 0;

        std::thread thread;
        std::atomic<bool> cancel = false;

        using state = sfx::media::state;

        quot ()
        {
            speaker.alpha = 64;
            mute.text.text = "mute";
            mute.kind = gui::button::toggle;
            mute.on = true;
            prev.repeating = true;
            next.repeating = true;
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
            speaker  .load(assets["speaker.128x096"].from(0));
            prev.icon.load(assets["icon.chevron.left.double.black.128x128"]);
            next.icon.load(assets["icon.chevron.right.double.black.128x128"]);
            play.icon.load(assets["player.black.play.64x64"]);
            Play.icon.load(assets["player.black.next.64x64"]);
            stop.icon.load(assets["player.black.pause.64x64"]);
            Stop.icon.load(assets["player.black.stop.64x64"]);
            speaker.play();
        }

        using idx = media::index;

        void reset (str title, array<idx> selected, array<str> const& links)
        {
            cancel = true;

            if (thread.joinable())
                thread.join();
            
            cancel = false;

            ready_players = 0;

            if (players.size() > 0)
            {
                auto& p = players(current);
                if (p.status == state::ready
                or  p.status == state::playing
                or  p.status == state::finished) 
                {
                    auto it = std::ranges::find(selected, p.index);
                    if (it != selected.end())
                    {
                        ready_players++;
                        players.rotate(0, current, current+1);
                        std::rotate(selected.begin(),
                            it, std::next(it));
                    }
                    else if (p.click)
                    {
                        ready_players++;
                        players.rotate(0, current, current+1);
                        selected.insert(0, p.index);
                    }
                }
            }

            int n = 0;
            for (auto index : selected)
                players(n++).reset(
                    index, links);

            playall = true;
            players.truncate(n);
            current = 0;

            for (auto & player : players)
            {
                player.hide();
                player.coord = players.coord.now.local();
                player.mute(mute.on);
            }

            if (players.size() > 0)
            {
                auto& p = players(current);
                if (p.status == state::ready
                or  p.status == state::playing
                or  p.status == state::finished) 
                    p.show();

                thread = std::thread([this, title]()
                {
                    for (auto& player: players) {
                        player.load(title, cancel);
                        if (cancel) break;
                    }
                });
            }
        }

        void currprev () { int n = ready_players; current = (current-1+n) % n; }
        void currnext () { int n = ready_players; current = (current+1  ) % n; }

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go(gui::time::infinity,
                         gui::time::infinity);

            if (what == &skin)
            {
                oneof.color = gui::skins[skin].touched.first;
            }
            if (what == &coord)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height*12/10;

                int l = h*27/100;
                int L = h*39/100;
                play.icon.padding = xyxy(L,l,L,l);
                stop.icon.padding = xyxy(L,l,L,l);
                Stop.icon.padding = xyxy(L,l,L,l);
                l = h*10/100;
                L = h*18/100;
                Play.icon.padding = xyxy(L,l,L,l);
                l = h*30/100;
                prev.icon.padding = xyxy(l,l,l,l);
                next.icon.padding = xyxy(l,l,l,l);

                int w = h*5/4;
                int d = h*1/5;
                int D = h*1/2 + 2*l;
                int y = 2*w*96/128;

                speaker .coord = xywh(0*w+d, 0+0*h+1*d, 2*w, y);
                mute    .coord = xywh(0*w+d, y+0*h+2*d, 2*w, h);
                play    .coord = xywh(0*w+d, y+1*h+3*d, 1*w, h);
                stop    .coord = xywh(0*w+d, y+1*h+3*d, 1*w, h);
                Play    .coord = xywh(1*w+d, y+1*h+3*d, 1*w, h);
                Stop    .coord = xywh(1*w+d, y+1*h+3*d, 1*w, h);
                oneof   .coord = xywh(0*w+d, y+2*h+3*d, 2*w, h);
                prev    .coord = xywh(0*w+d, y+3*h+3*d, 1*w, h);
                next    .coord = xywh(1*w+d, y+3*h+3*d, 1*w, h);
                players .coord = xyxy(2*w+D, 0, W-D, H);

                for (auto & player : players)
                    player.coord = players.
                        coord.now.local();
            }

            if (what == &timer)
            {
                while (ready_players < players.size() and
                    players(ready_players).status == state::ready)
                    ready_players++;

                if (ready_players > 0)
                {
                    switch(players(current).status) {
                    case state::failed:
                        players(current).status = state::finished;
                        players(current).show(smoothly);
                        break;
                    case state::ready:
                        players(current).play();
                        players(current).show(smoothly);
                        break;
                    case state::finished:
                        if (players.size() < 2
                            or not playall)
                            break;
                        players(current).stop();
                        players(current).hide(smoothly); currnext();
                        players(current).show(smoothly); if (current != 0)
                        players(current).play(); else playall = false;
                        break;
                    default:
                        break;
                    }

                    bool playing = players(current).status == state::playing;
                    play.hide(playing);
                    Play.hide(playing);
                    stop.show(playing);
                    Stop.show(playing);
                    play.enabled = true;
                    Play.enabled = true;

                    prev.enabled  = ready_players > 1;
                    next.enabled  = ready_players > 1;
                    oneof.show     (ready_players > 0);
                    oneof.text =
                        std::to_string(current+1) + " of " +
                        std::to_string(ready_players);

                    if (mute.on or not playing)
                        speaker.alpha = 64; else
                    if (speaker.alpha.to  == 128 and
                        speaker.alpha.now == 128)
                        speaker.alpha.go( 64, gui::time(3000)); else
                    if (speaker.alpha.to  ==  64 and
                        speaker.alpha.now ==  64)
                        speaker.alpha.go(128, gui::time(3000));
                }
                else
                {
                    speaker.alpha = 64;
                    play.show();
                    Play.show();
                    play.enabled = false;
                    Play.enabled = false;
                    stop.hide();
                    Stop.hide();
                    oneof.hide();
                    prev.enabled = false;
                    next.enabled = false;
                }
            }

            if (what == &players)
            {
                clicked = players.notifier->clicked;
                notify();
            }

            int pp = ready_players;

            if (what == &Stop) playall = false;
            if (what == &stop) playall = false;
            if (what == &play) playall = false;
            if (what == &Play) playall = true;

            if (what == &play) mute.on = false;
            if (what == &Play) mute.on = false;

            if (what == &Stop and pp > 0) players(current).stop();
            if (what == &stop and pp > 0) players(current).stop();
            if (what == &play and pp > 0) players(current).play();
            if (what == &Play and pp > 0) players(current).play();

            if (what == &mute)
            {
                for (auto& player: players)
                    player.mute(mute.on);
                
                notify(what);
            }

            if (what == &prev)
            {
                players(current).stop();
                players(current).hide(instantly);
                current = (current-1+pp) % pp;
                players(current).show(instantly); if (playall)
                players(current).play();
            }
            if (what == &next)
            {
                players(current).stop();
                players(current).hide(instantly);
                current = (current+1) % pp;
                players(current).show(instantly); if (playall)
                players(current).play();
            }
            if (what == &Stop and current != 0)
            {
                players(current).stop();
                players(current).hide(smoothly);
                current = 0;
                players(current).show(smoothly);
            }
        }
    };
}

