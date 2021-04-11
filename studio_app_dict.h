#pragma once
#include "app_dict.h"
namespace studio::dict
{
    struct area : gui::widget<area>
    {
        gui::console log;

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                log.coord = coord.now.local();
            }
        }
    };

    struct studio : gui::widget<studio>
    {
        app::dict::app app;
        gui::area<area> area;
        gui::splitter splitter;

        studio () { app::dict::log = &area.object.log; }

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
        std::multimap<int, int> entry_media; int total_media = 0;

        media::
        data::out::storage storage("../data/app_dict");
        dat::out::file entry_index("../data/app_dict/entry_index.dat");
        dat::out::file media_index("../data/app_dict/media_index.dat");

        for (auto & r : resources)
        {
            media::data::location location;

            if (r.kind == "audio" and
                r.entries.size() == 1) // title already there
                r.entries = eng::parser::entries(r.title);

            for (str entry : r.entries)
            {
                if (entry.ends_with("}")) {
                    str sense; entry.split_by("{",
                        entry, sense); entry.strip(); }

                entry.replace_all("_", "");

                auto range = eng::vocabulary::find(entry);
                if (range.empty()) continue;
                int n = range.offset();

                if (vocabulary[n].redirect >= 0) n =
                    vocabulary[n].redirect;

                if (location == media::data::location{}) {
                    location = storage.add(r);
                    media_index << r.kind;
                    media_index << r.title;
                    media_index << r.comment;
                    media_index << r.credit;
                    media_index << r.options;
                    media_index << location;
                    total_media++;
                }

                entry_media.emplace(n, total_media-1);
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
