#pragma once
#include "app.h"
namespace app::two
{
    struct view:
    widget<view>
    {
        property<bool> playmode = false;
        property<bool> translated = false;

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
              "app::two::path", ""));
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

        void play ()
        {
            medio.done();
        }
        void stop ()
        {
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

        void go (str)
        {
        }
    };
}

