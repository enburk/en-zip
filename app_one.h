#pragma once
#include "app.h"
#include "app_one_stage.h"
namespace app::one
{
    struct app:
    widget<app>
    {
        stage stage;

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

        void reload () try
        {
            stage.fill();
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

