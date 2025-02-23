#pragma once
#include "app.h"
#include "app_one_stage.h"
namespace app::one
{
    struct view:
    widget<view>
    {
        stage  stages[3];
        stage& stage = stages[1];
        str    where;

        property<bool> playmode = false;
        property<bool> translated = false;

        sfx::media::medio medio;
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
            stage.play();
            notify();
        }
        void stop ()
        {
            medio.stop();
            stage.stop();
            notify();
        }

        void next ()
        {
            stage.next();
            notify();
        }
        void prev ()
        {
            medio.stop();
            stage.stop();
            stage.prev();
            notify();
        }

        void Next ()
        {
            if (not stage.topic) return;
            auto topic = stage.topic->next();
            if (not topic) return;

            stop();

            //std::rotate(
            //std::begin(stages),
            //std::begin(stages)+1,
            //std::end  (stages));

            go(topic->path);
            play();
        }
        void Prev ()
        {
            if (not stage.topic) return;
            auto topic = stage.topic->prev();
            if (not topic) return;

            stop();

            //std::rotate(
            //std::rbegin(stages),
            //std::rbegin(stages)+1,
            //std::rend  (stages));

            go(topic->path);
            play();
        }

        void go (str path, bool app_shown = true)
        {
            stage.go(path, shown());

            where =
            stage.topic and
            stage.topic->parent?
            stage.topic->parent->path : red(bold(path));
            where.replace_all("/", blue("/"));
            where.replace_all("''Extra''",
                extracolor("Extra"));

            if(
            stage.topic and
            stage.topic->parent) sys::settings::save("app::one::path",
            stage.topic->parent->path);
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                stage.coord = coord.now.local();
            }

            if (what == &playing)
            {
                switch(stage.status) {
                case state::ready:
                case state::paused:
                    stage.show();
                    stage.play();
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

