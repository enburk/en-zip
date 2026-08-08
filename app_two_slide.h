#pragma once
#include "app.h"
#include "app_two_entry.h"
namespace app::two
{
    struct slide:
    widget<slide>
    {
        array<entry*> entries;
        property<byte> volume = 255;
        property<bool> mute = false;
  //      sfx::media::medio medio;
        content::unit* topic = nullptr;
        int current = 0;

        void reset ()
        {
//            medio.done();
            entries.clear();
            current = 0;
        }

        void show ()
        {
            for (auto& e: entries)
            e->show();
        }

        //void hide ()
        //{
        //    for (auto& e: entries)
        //    e->stop(),
        //    e->hide();
        //}

        //void play ()
        //{
        //    medio.stay();
        //    medio.play();
        //}

        //void stop ()
        //{
        //    if (medio.stop()
        //    and not entries.empty())
        //    entries[current]->stop();
        //}

        void on_change (void* what) override
        {
            if (what == &volume)
                for (auto e: entries)
                    e->volume = volume;

            if (what == &mute)
                for (auto e: entries)
                    e->mute = mute;
        }
    };
}