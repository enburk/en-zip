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
        view () { for (int i=0; i<N; i++) stages[i].stop(); }

        property<bool> playmode = false;
        property<bool> translated = false;

        str where;
        sfx::media::medio medio;
        gui::timer fetcher;
        gui::timer nexter;

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

        void reload () try
        {
            go(sys::settings::load(
              "app::one::path", ""));
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

        void play ()
        {
            medio.stay();
            medio.play();
            stage().play();
            notify();
        }
        void stop ()
        {
            medio.stop();
            stage().stop();
            notify();
        }

        void next ()
        {
            stage().next();
            notify();
        }
        void prev ()
        {
            medio.stop();
            stage().stop();
            stage().prev();
            notify();
        }

        void Next ()
        {
            if (not stage().topic) return;
            auto topic = stage().topic->next();
            if (not topic) return;

            stop(); stage().Stop();

            xywh
            r = stages.coord.now.local();
            r += xy(L*r.w, 0);

            stages[0].hide();
            stages[0].coord = r;
            stages.rotate(0,1,N);
            place(1s);

            go(topic->path);
            stage().show();
            play();
        }
        void Prev ()
        {
            if (not stage().topic) return;
            auto topic = stage().topic->prev();
            if (not topic) return;

            stop(); stage().Stop();

            xywh
            r = stages.coord.now.local();
            r -= xy(L*r.w, 0);

            stages[N-1].hide();
            stages[N-1].coord = r;
            stages.rotate(0, N-1, N);
            place(1s);

            go(topic->path);
            stage().show();
            play();
        }

        void go (str path, bool app_shown = true)
        {
            stage().go(course.find(path), shown());

            where =
            stage().topic and
            stage().topic->parent?
            stage().topic->parent->path : red(bold(path));
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

            if (what == &playing)
            {
                switch(stage().status) {
                case state::ready:
                case state::paused:
                    stage().show();
                    stage().play();
                    notify();
                    break;
                case state::finished:
                    nexter.setup(2s);
                    medio.done();
                    notify();
                    break;
                default:
                    break;
                }
            }

            if (what == &fetcher)
            {
                fetcher.stop();
                if (stage().topic) fetch().go(
                    stage().topic->next());
            }

            if (what == &nexter)
            {
                nexter.stop();
                Next();
                notify();
            }

            if (what == &playmode)
                for (auto& s: stages)
                    s.playmode =
                      playmode;

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
        }
    };
}

