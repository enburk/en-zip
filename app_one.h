#pragma once
#include "app.h"
#include "app_one_stage.h"
namespace app::one
{
    struct view:
    widget<view>
    {
        const int L = 1;
        const int N = 2*L+1;
        widgetarium<stage> stages;
        stage& fetch() { return stages[L+1]; }
        stage& stage() { return stages[L+0]; }

        property<bool> playmode = false;
        property<bool> translated = false;

        str where;
        sfx::media::medio medio;
        gui::timer fetcher;
        bool played = true;
        int clicked = 0;

        using state = sfx::media::state;

#define using(x) decltype(medio.x)& x = medio.x;
        using(mute)
        using(volume)
        using(loading)
        using(playing)
        using(resolution)
        using(duration)
        using(elapsed)
        using(status)
        using(error)
        #undef using

        using unit = content::unit;

        view ()
        {
            // just create N of them
            for (int i=0; i<N; i++)
                stages[i].stop();

            playmode = sys::settings::load("app::one::mode", 1);
        }

        void reload () try
        {
            go(sys::settings::load(
              "app::one::path", ""));
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

        void start ()
        {
            if (playmode.now)
            {
                if (played)
                    play();
            }
            else
            stage().show_all();
            stage().show();
        }
        void halt ()
        {
            played = status == state::playing;
            stop();
        }

        void play ()
        {
            playmode = true;
            medio.stay();
            medio.play();
            stage().play();
            notify(&status);
        }
        void Play ()
        {
            if (not playmode)
            stage().replay();
            play();
        }
        void stop ()
        {
            medio.stop();
            stage().stop();
            notify(&status);
        }
        void Stop ()
        {
            playmode = false;
            stop();
        }

        void next ()
        {
            stage().next();
        }
        void prev ()
        {
            stop();
            stage().prev();
        }

        void Next ()
        {
            if (not stage().topic) return;
            auto topic = stage().topic->next_theme_first_topic();
            if (not topic) return;

            halt();

            xywh
            r = stages.coord.now.local();
            r += xy(L*r.w, 0);

            stages[0].hide();
            stages[0].coord = r;
            stages.rotate(0,1,N);
            place(1s);

            if (stage().topic
            and stage().topic->parent == topic->parent)
            go (stage().topic); else
            go (topic);
        }
        void Prev ()
        {
            if (not stage().topic) return;
            auto topic = stage().topic->prev_theme_first_topic();
            if (not topic) return;

            halt();

            xywh
            r = stages.coord.now.local();
            r -= xy(L*r.w, 0);

            stages[N-1].hide();
            stages[N-1].coord = r;
            stages.rotate(0, N-1, N);
            place(1s);

            if (stage().status == state::finished)
                stage().replay();

            if (stage().topic
            and stage().topic->parent == topic->parent)
            go (stage().topic); else
            go (topic);
        }

        void go (unit* x)
        {
            stage().go(x);
            start();

            where =
            stage().topic and
            stage().topic->parent?
            stage().topic->parent->path : red(bold(x ? x->path : "null"));
            where.replace_all("/", blue("/"));
            where.replace_all("''Extra''",
                extracolor("Extra"));

            if(
            stage().topic and
            stage().topic->parent) sys::settings::save("app::one::path",
            stage().topic->parent->path);

            if (not playmode.now)
                fetcher.setup(2s);
        }
        void go (str path) { go(course.find(path)); }

        void speedup ()
        {
            for (auto& stage: stages)
            stage.speedup();
        }

        void place (gui::time t={})
        {
            xywh r = stages.coord.now.local();
            r -= xy(L*r.w, 0);

            for (int i=0; i<N; i++) stages[i].move_to(
            r + xy(i*r.w, 0), t);
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                stages.coord = coord.now.local();
                place();
            }

            if (what == &alpha
            and alpha.to  == 255
            and alpha.now == 255)
                start();

            if (what == &alpha
            and alpha.to  == 0
            and alpha.now == 0)
                halt();

            if (what == &playing)
            {
                switch(stage().status) {
                case state::ready:
                case state::paused:
                    stage().show();
                    stage().play();
                    break;
                case state::finished:
                    Next();
                    play();
                    break;
                default:
                    break;
                }
            }

            if (what == &fetcher)
            {
                fetcher.stop();

                if (not stage().topic) return;
                auto topic = stage().topic->next_theme_first_topic();
                if (not topic) return;

                if (fetch().topic == nullptr
                or  fetch().topic->parent != topic->parent)
                    fetch().go(topic);
            }

            if (what == &stages)
            {
                clicked = stages.
                notifier->clicked;
                notify();
            }

            if (what == &playmode)
                sys::settings::save("app::mode",
                    playmode.now ? 1 : 0);

            if (what == &translated)
                for (auto& s: stages)
                    s.translated =
                      translated;

            if (what == &volume)
                for (auto& s: stages)
                    s.volume =
                      volume;

            if (what == &mute)
                for (auto& s: stages)
                    s.mute =
                      mute;

            notify(&status);
        }
    };
}

