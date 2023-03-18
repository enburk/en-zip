#pragma once
#include "app_dic_html.h"
#include "app_dic_media.h"
#include "app_dic_media_audio.h"
namespace app::dic::left
{
    struct quot:
    widget<quot>
    {
        gui::icon speaker;
        gui::button mute;
        gui::button play, Play;
        gui::button stop, Stop;
        gui::button prev, next;
        gui::text::view oneof;

        sfx::media::
        sequencer<audio::player> audio;
        gui::property<gui::time> timer;
        using idx = media::index;
        int clicked = 0;

        quot ()
        {
            speaker.alpha = 64;
            mute.text.text = "mute";
            mute.kind = gui::button::toggle;
            mute.on = true;
            prev.repeating = true;
            next.repeating = true;
        }

        void reload ()
        {
            speaker  .load(assets["speaker.128x096"]);
            prev.icon.load(assets["icon.chevron.left.double.black.128x128"]);
            next.icon.load(assets["icon.chevron.right.double.black.128x128"]);
            play.icon.load(assets["player.black.play.64x64"]);
            Play.icon.load(assets["player.black.next.64x64"]);
            stop.icon.load(assets["player.black.pause.64x64"]);
            Stop.icon.load(assets["player.black.stop.64x64"]);
        }

        void reset (array<idx> audios, array<str> links)
        {
            auto& players = audio.players;
            auto& current = audio.current;

            if (players.size() > 0)
            {
                auto it = std::ranges::find(
                audios, players[current].index);
                if (it != audios.end())
                {
                    players.rotate(0, current, current+1);
                    std::rotate(audios.begin(), it, std::next(it));
                }
            }

            int n = audios.size();
            for (int i=0; i<n; i++)
            players[i].load(audios[i], links);
            players.truncate(n);
            current = 0;

            for (auto& player: players)
            {
                player.hide();
                player.coord = players.coord.now.local();
                player.mute = mute.on;
            }

            using state = sfx::media::state;

            if (players.size() > 0)
            if (players[0].status == state::ready   or
                players[0].status == state::playing or
                players[0].status == state::finished)
                players[0].show();

            audio.Play();
        }

        bool text_selected () { return
            audio.players.size() > 0 and
            audio.players[audio.current].
            text.view.selected() != ""; }

        void on_change (void* what) override
        {
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

                speaker.coord = xywh(0*w+d, 0+0*h+1*d, 2*w, y);
                mute   .coord = xywh(0*w+d, y+0*h+2*d, 2*w, h);
                play   .coord = xywh(0*w+d, y+1*h+3*d, 1*w, h);
                stop   .coord = xywh(0*w+d, y+1*h+3*d, 1*w, h);
                Play   .coord = xywh(1*w+d, y+1*h+3*d, 1*w, h);
                Stop   .coord = xywh(1*w+d, y+1*h+3*d, 1*w, h);
                oneof  .coord = xywh(0*w+d, y+2*h+3*d, 2*w, h);
                prev   .coord = xywh(0*w+d, y+3*h+3*d, 1*w, h);
                next   .coord = xywh(1*w+d, y+3*h+3*d, 1*w, h);
                audio  .coord = xyxy(2*w+D, 0, W-D, H);
            }

            using gui::time;
            if (timer.now == time{})
                timer.go(time::infinity,
                         time::infinity);

            if (what == &timer)
            {
                int nn = audio.players.size();
                if (nn > 0)
                {
                    play.hide(audio.playing);
                    Play.hide(audio.playing);
                    stop.show(audio.playing);
                    Stop.show(audio.playing);
                    play.enabled = true;
                    Play.enabled = true;

                    prev.enabled  = nn > 1;
                    next.enabled  = nn > 1;
                    oneof.show     (nn > 0);
                    oneof.text =
                    std::to_string(audio.current+1) + " of " +
                    std::to_string(nn);

                    if (not audio.playing
                    or  speaker.alpha.to  == 128
                    and speaker.alpha.now == 128)
                        speaker.alpha.go( 64, 3s);
                    else
                    if (speaker.alpha.to  ==  64
                    and speaker.alpha.now ==  64)
                        speaker.alpha.go(128, 3s);
                }
                else
                {
                    play.show();
                    Play.show();
                    stop.hide();
                    Stop.hide();
                    oneof.hide();
                    play.enabled = false;
                    Play.enabled = false;
                    prev.enabled = false;
                    next.enabled = false;
                    speaker.alpha = 64;
                }
            }

            if (what == &audio)
                clicked = audio.clicked,
                notify();

            if (what == &mute)
                audio.mute = mute.on,
                notify(what);

            if (what == &play) mute.on = false;
            if (what == &Play) mute.on = false;

            if (what == &Stop) audio.Stop();
            if (what == &stop) audio.stop();
            if (what == &play) audio.play();
            if (what == &Play) audio.Play();
            if (what == &prev) audio.prev();
            if (what == &next) audio.next();
        }
    };
}

