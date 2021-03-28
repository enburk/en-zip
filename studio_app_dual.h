#pragma once
#include "app_dual.h"
namespace studio::dual
{
    struct area : gui::widget<area>
    {
    };

    struct studio : gui::widget<studio>
    {
        app::dual::app app;
        gui::area<area> area;
        gui::splitter splitter;

        void reload () { app.reload(); }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int l = gui::metrics::line::width*3;
                int w = W/2;
                int d = 2*l;

                splitter.coord = XYWH(W-w-d, 0, 2*d, H);
                splitter.lower = 80'00 * W / 100'00;
                splitter.upper =  8'00 * W / 100'00;

                area.coord = XYWH(0, 0, W-w, H);
                app .coord = XYWH(W-w, 0, w, H);
            }
        }

        void on_focus (bool on) override { app.on_focus(on); }
        void on_keyboard_input (str symbol) override { app.on_keyboard_input(symbol); }
        void on_key_pressed (str key, bool down) override { app.on_key_pressed(key,down); }
    };

    inline void compile (array<media::resource> & resources)
    {
        std::multimap<int, int> entry_media; int total_media = 0;

        media::
        data::out::storage storage("../data/app_dual");
        dat::out::file entry_index("../data/app_dual/entry_index.dat");
        dat::out::file media_index("../data/app_dual/media_index.dat");

        for (auto & r : resources)
        {
            if (r.entries.size() == 0)
                r.entries += r.title;

            str qrop;
            for (str option : r.options)
                if (option.starts_with("qrop "))
                    qrop = option.from(5);

            for (str & option : r.options)
                if (option.starts_with("crop "))
                    option = "crop " + qrop;

            //media::data::location location;

            for (str entry : r.entries)
            {
            }
        }

        entry_index << total_media;
        entry_index << (int)(entry_media.size());
        for (auto [entry, media] : entry_media) {
            entry_index << entry;
            entry_index << media;
        }
    }
}

