#pragma once
#include "studio_app_dic_area.h"
namespace studio::dic
{
    eng::dictionary dictionary;
    std::mutex dictionary_mutex;
    void load_dictionary (gui::console& console)
    {
        std::lock_guard lock{dictionary_mutex};
        if (not dictionary.entries.empty()) return;

        console.clear();
        console << red("load dictionary: file not found");

        std::filesystem::path dir = "../data";
        if (!std::filesystem::exists(dir/"dictionary_indices.dat")) return;
        if (!std::filesystem::exists(dir/"dictionary_entries.dat")) return;
        std::ifstream indices_stream(dir/"dictionary_indices.dat", std::ios::binary);
        std::ifstream entries_stream(dir/"dictionary_entries.dat", std::ios::binary);
        dat::in::pool indices_pool;
        dat::in::pool entries_pool;

        int m = 0;
        int n = app::dic::vocabulary.size();
        dictionary.entries.resize(n);
        dictionary.indices.resize(n);

        for (int i=0; i<n; i++) try
        {
            indices_pool.offset = 0;
            indices_stream.seekg(i*eng::dictionary::index::size, std::ios::beg);
            indices_pool.bytes.resize(eng::dictionary::index::size);
            indices_stream.read((char*)(indices_pool.bytes.data()),
            eng::dictionary::index::size);
            eng::dictionary::index& index =
            dictionary.indices[i];
            index << indices_pool;

            dictionary.entries[i].title =
            app::dic::vocabulary[i].title;

            int j = i+1;
            if (i == n-1
            or  i % 1000 == 0) {
                console.clear();
                console << gray("load: " 
                + std::to_string(j) + " of "
                + std::to_string(n) + " entries ("
                + std::to_string(m) + " unique)"); }

            if (index.redirect >= 0
            or  index.length == 0)
                continue;
            m++;

            entries_pool.offset = 0;
            entries_stream.seekg(index.offset, std::ios::beg);
            entries_pool.bytes.resize(index.length);
            entries_stream.read((char*)(entries_pool.bytes.data()), index.length);
            dictionary.entries[i] << entries_pool;
        }
        catch (std::exception const& e) {
        console << red(e.what()); }
    }

    struct studio:
    widget<studio>
    {
        area area;
        app::dic::app app; // after area
        gui::splitter splitter;

        studio () { reload(); }

        void reload ()
        {
            timing t0;
            locations.clear();
            dat::in::pool pool("../data/app_dict/locationary.dat");
            while (not pool.done()) {
                int source = pool.get_int();
                int offset = pool.get_int();
                str path = pool.get_string();
                locations[source][offset] = path;
            }

            timing t1;
            app::dic::logs::times << gray(monospace(
            "studio locationary " + format(t1-t0) + " sec<br>" +
            "studio load total  " + format(t1-t0) + " sec"));
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int d = gui::metrics::line::width*10;

                splitter.lower = W * 25'00 / 100'00;
                splitter.upper = W * 75'00 / 100'00;
                str s = "studio::dic::splitter.permyriad";
                int p = sys::settings::load(s, 55'00);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower, splitter.upper);
                splitter.coord = xyxy(x-d, 0, x+d, H);

                area.coord = xyxy(0, 0, x, H);
                app.coord = xyxy(x, 0, W, H);
            }

            if (what == &splitter) {
                sys::settings::save(
                "studio::dic::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }

            if (what == &area)
            {
                focus = &app.list;

                app.list.select(area.clicked);

                if (area.search.alpha.to > 0)
                {
                    str s = area.search.object.request.now;
                    if (s == "") return;

                    array<gui::text::range> highlights;
                    auto& view = app.left.card.object.text;
                    auto& text = view.model.now->block;
                    for (auto& line:  text.lines)
                    for (auto& token: line.tokens)
                    if (token.text.contains(s))
                    highlights += token.range;
                    view.highlights =
                        highlights;
                }
            }
        }
    };
}
