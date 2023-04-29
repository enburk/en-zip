#pragma once
#include "app.h"
#include "app_one_entry.h"
namespace app::one
{
    struct slide:
    widget<slide>
    {
        array<entry*> entries;
        sfx::media::medio medio;
        gui::time instantly = 50ms;
        gui::time smoothly = 500ms;
        gui::time swiftly  = 100ms;
        int current = 0;

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

        ~slide() { reset(); }

        void reset ()
        {
            medio.done();
            entries.clear();
            current = 0;
        }

        void show ()
        {
            for (auto& e: entries)
            e->show();
        }

        void hide ()
        {
            for (auto& e: entries)
            e->stop(),
            e->hide();
        }

        void play ()
        {
            medio.stay();
            medio.play();
        }

        void stop ()
        {
            if (medio.stop()
            and not entries.empty())
            entries[current]->stop();
        }

        void on_change (void* what) override
        {
            if (what == &playing)
            {
                if (not entries.empty())
                switch(entries[current]->status) {
                case state::ready:
                case state::paused:
                    entries[current]->show(smoothly);
                    entries[current]->play();
                    break;
                case state::finished:
                    if (current >= entries.size()-1) {
                        medio.done();
                        break;
                    }
                    current++;
                    entries[current]->show(smoothly);
                    entries[current]->play();
                    break;
                default:
                    break;
                }
            }

            if (what == &volume)
                for (auto e: entries)
                    e->volume = volume;

            if (what == &mute)
                for (auto e: entries)
                    e->mute = mute;
        }
    };
}