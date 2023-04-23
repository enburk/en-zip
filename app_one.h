#pragma once
#include "app.h"
#include "app_one_stage.h"
namespace app::one
{
    struct app:
    widget<app>
    {
        stage  stages[3];
        stage& stage = stages[1];
        str    where;

        sfx::media::medio medio;

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
              "app:one::path", ""));
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

        void play ()
        {
            medio.stay();
            medio.play();
            stage.play();
        }
        void stop ()
        {
            medio.stop();
            stage.stop();
        }

        void next ()
        {
        }
        void prev ()
        {
        }

        void Next ()
        {
        }
        void Prev ()
        {
        }

        void go (str path)
        {
            stage.where = course.find(path);
            stage.fill();
            where =
            stage.theme ?
            stage.theme->path:
            red(bold(path));
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
                    break;
                case state::finished:
                    //stage.next();
                    stage.show();
                    stage.play();
                    break;
                default:
                    break;
                }
            }
        }
    };
}

