#pragma once
#include "app_dict.h"
namespace studio::dict
{
    struct area : gui::widget<area>
    {
    };

    struct studio : gui::widget<studio>
    {
        app::dict::app app;
        gui::area<area> area;
        gui::splitter splitter;

        void reload () { app.reload(); }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int w = W/2;
                int d = 2*l;

                splitter.coord = XYWH(W-w-d, 0, 2*d, H);
                splitter.lower = 8'000 * W / 10'000;
                splitter.upper =   800 * W / 10'000;

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
        std::map<int, array<media::data::location>> mediae;

        media::data::out::storage storage ("../data/app_dict");
        dat::out::file index ("../data/app_dict/index.dat");

        for (auto & r : resources)
        {
            if (r.entries.size() == 0)
                r.entries += r.title;

            media::data::location location;

            for (str entry : r.entries)
            {
                if (auto range = eng::vocabulary::find(entry); range)
                {
                    if (location == media::data::location{})
                        location = storage.add(r);

                    mediae[range.offset] += location;
                }
            }
        }

        for (auto [entry, locations] : mediae) {
            index << entry;
            index << locations;
        }
    }
}
