#pragma once
#include "app.h"
namespace studio
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
        sys::in::pool indices_pool;
        sys::in::pool entries_pool;

        int m = 0;
        int n = app::vocabulary.size();
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
               app::vocabulary[i].title;

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
}
