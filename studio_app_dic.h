#pragma once
#include "app_dic.h"
#include "eng_parser.h"
#include "eng_unittest.h"
#include "media_data.h"
namespace studio::dic
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
        app::dic::app app;
        gui::area<area> area;
        gui::splitter splitter;

        studio () { app::dic::log = area.object.log; }

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

    inline void compile (array<media::resource> const& resources, gui::console& report)
    {
        std::multimap<int, int> entry_media; int total_media = 0;

        media::
        data::out::storage storage("../data/app_dict");
        dat::out::file entry_index("../data/app_dict/entry_index.dat");
        dat::out::file media_index("../data/app_dict/media_index.dat");
        dat::out::file assets_data("../data/app_dict/assets.dat");
        eng::vocabulary vocabulary("../data/vocabulary.dat");
        array<media::resource> assets;

        if (not eng::unittest::proceed(vocabulary, report)) return;

        using res = media::resource const*;
        std::unordered_map<str, array<res>> entries2resources;
        std::unordered_map<res, array<str>> resources2entries;

        for (auto & r : resources)
        {
            if (r.title == "speaker.128x096"
            or  r.title == "player.black.play.64x64"
            or  r.title == "player.black.pause.64x64"
            or  r.title == "player.black.next.64x64"
            or  r.title == "player.black.stop.64x64"
            or  r.title == "icon.chevron.up.black.128x128"
            or  r.title == "icon.chevron.down.black.128x128"
            or  r.title == "icon.chevron.right.black.128x128"
            or  r.title == "icon.chevron.left.black.128x128"
            or  r.title == "icon.chevron.up.double.black.128x128"
            or  r.title == "icon.chevron.down.double.black.128x128"
            or  r.title == "icon.chevron.right.double.black.128x128"
            or  r.title == "icon.chevron.left.double.black.128x128"
            or  r.title == "icon.settings.black.192x192")
            {
                assets += r;
                continue;
            }

            array<str> entries = r.entries;

            if (r.kind == "audio" and not
                r.options.contains("=") and
                r.entries.size() == 1) // title already there
                entries = eng::parser::entries(vocabulary, r.title);

            if (r.kind == "audio" and
                r.entries.size() == 1 and
                r.entries.front() == r.title)
            {
                str s = r.entries.front();  s.trimr("!?");
                if(s.starts_with("a "    )) entries += s.from (2); else
                if(s.starts_with("an "   )) entries += s.from (3); else
                if(s.starts_with("the "  )) entries += s.from (4); else
                if(s.starts_with("to be ")) entries += s.from (7); else
                if(s.starts_with("to "   )) entries += s.from (3); else
                {}
            }

            entries.deduplicate();
            for(auto& entry: entries)
            if (vocabulary.index(entry))
            entries2resources[entry] += &r;
        }

        for (auto& [entry, resources]: entries2resources)
        {
            if (resources.size() > 99)
            {
                resources.sort([]
                (auto& r1, auto& r2){ return
                    r1->title.size() <
                    r2->title.size();
                    });
                resources.resize(99);
            }
            for (auto r: resources)
            resources2entries[r] += entry;
        }

        for (auto& r: resources)
        {
            auto it = resources2entries.find(&r);
            if (it == resources2entries.end()) {
                report << yellow(r.title) +
                red("[" + str(r.entries, "][") + "]");
                continue;
            }

            auto[location, new_one] = storage.add(r);

            media_index << r.kind;
            media_index << r.title;
            media_index << r.comment;
            media_index << r.credit;
            media_index << r.options;
            media_index << location;
            total_media++;

            array<str> entries;
            array<str> skipped;

            for (str entry: it->second)
            {
                if (entry.ends_with("}")) {
                str sense; entry.split_by("{",
                    entry, sense); entry.strip(); }

                entry.replace_all("_", "");

                auto index = vocabulary.index(entry);
                if (!index) continue;
                int n = *index;

                if (vocabulary[n].redirect >= 0) n =
                    vocabulary[n].redirect;

                entry_media.emplace(n, total_media-1);

                entries += entry;
            }

            for (auto& entry : r.entries)
                if (not entries.contains(entry)
                    and entry != r.title)
                    skipped += entry;

            if (new_one) report <<  purple(r.title) +
            blue ("[" + str(entries,   "][") + "]") +
            red  ("[" + str(skipped,   "][") + "]") +
            green("{" + str(r.options, "}{") + "}");

            if (new_one and false) { report << r.id;
            report << "location.source: " + std::to_string(location.source);
            report << "location.offset: " + std::to_string(location.offset);
            report << "location.length: " + std::to_string(location.length); }
        }

        entry_index << total_media;
        entry_index << (int)(entry_media.size());
        for (auto [entry, media] : entry_media) {
            entry_index << entry;
            entry_index << media;
        }

        assets_data << assets.size();
        for (auto & r : assets) {
            assets_data << r.title;
            assets_data << dat::in::bytes(r.path).value();
        }
    }
}
