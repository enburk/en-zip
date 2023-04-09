#pragma once
#include "media_scan.h"
#include "media_storage.h"
namespace media::in
{
    struct data
    {
        std::map<str, array<sys::byte>> assets;
        array<entry_index> entries_dic;
        array<entry_index> entries_one;
        array<entry_index> entries_two;
        array<media_index> media_index;
        array<str>         media_paths;

        void reload () try
        {
            logs::out << dark(bold("LOAD..."));

            entries_dic.clear();
            entries_one.clear();
            entries_two.clear();
            media_index.clear();
            assets.clear();

            path dir = "../data/media";
            auto
            f = sys::in::file(dir/"entries_dic.dat"); f >> entries_dic;
            f = sys::in::file(dir/"entries_one.dat"); f >> entries_one;
            f = sys::in::file(dir/"entries_two.dat"); f >> entries_two;
            f = sys::in::file(dir/"media_index.dat"); f >> media_index;
            f = sys::in::file(dir/"media_paths.dat"); f >> media_paths;
            f = sys::in::file(dir/"assets.dat");
            int nn = f.pool.get_int();
            for (int i=0; i<nn; i++) {
                auto title = f.pool.get_string();
                auto bytes = f.pool.get_bytes();
                assets[title] = std::vector<sys::byte>(
                    bytes.data,
                    bytes.data +
                    bytes.size);
            }
            nn = media_paths.size();
            for (int i=0; i<nn; i++)
                media_index[i].path =
                media_paths[i];

            logs::out << dark(bold("LOAD OK"));
        }
        catch(std:: exception const& e) {
        logs::err << red(bold(
            e.what())); }
    };
}
namespace app
{
    media::in::data mediadata;
    auto & assets = mediadata.assets;
}
namespace media::out
{
    struct data
    {
        storage storage;
        array<resource> resources;
        std::set<resource*> new_ones;
        std::set<resource*> assets;
        array<entry_index> entries_dic;
        array<entry_index> entries_one;
        array<entry_index> entries_two;
        array<media_index> media_index;
        array<str>         media_paths;

        data () : storage("../data/media")
        {
            setlocale(LC_ALL,"en_US.utf8");

            using logs::out;
            using logs::err;

            report::id2path.clear();
            report::unidentified.clear();
            report::updated = false;

            resources.reserve(128*1024);
            scan::dataelog = std::ofstream("datae.txt");
            scan::scan("../datae", resources);
            scan::dataelog = std::ofstream{};

            auto&   unidentified = 
            report::unidentified;
            if (not unidentified.empty()) {
            err << "unidentified files:";
            for (auto path: unidentified)
            err << path.string(); }

            for (auto& [id, paths]: report::id2path)
            if  (paths.size() > 1) {
            err << "files with same id: " + id;
            for (auto path: paths)
            err << path.string(); }
        }
        int add (resource* r, str cropkind)
        {
            int& index = 
            cropkind == "qrop"
            and r->qropped ?
                r->iqrop:
                r->index;

            if (index != -1)
                return index;

            index = media_index.size();

            auto[location, new_one] = storage.add(*r, cropkind);

            if (new_one)
                new_ones.insert(r),
                logs::out << html(
                r->path.string());

            media::index m;
            m.kind     = r->kind;
            m.title    = r->title;
            m.sense    = r->sense;
            m.comment  = r->comment;
            m.credit   = r->credit;
            m.options  = r->options;
            m.location = location;

            media_index += m;
            media_paths += r->path.string();

            return index;
        }
        void dic_add(int entry, resource* r) { entries_dic.emplace_back(entry, add(r, "crop")); }
        void one_add(int entry, resource* r) { entries_one.emplace_back(entry, add(r, "qrop")); }
        void two_add(int entry, resource* r) { entries_two.emplace_back(entry, add(r, "qrop")); }
        void save () try
        {
            logs::out << dark(bold("SAVE..."));

            std::ranges::stable_sort(entries_dic, {}, &::media::entry_index::entry);
            std::ranges::stable_sort(entries_one, {}, &::media::entry_index::entry);
            std::ranges::stable_sort(entries_two, {}, &::media::entry_index::entry);

            bool up1 = entries_dic != app::mediadata.entries_dic;
            bool up2 = entries_one != app::mediadata.entries_one;
            bool up3 = entries_two != app::mediadata.entries_two;
            bool up4 = media_index != app::mediadata.media_index;
            bool up5 = media_paths != app::mediadata.media_paths;

            path dir = "../data/media";
            if (up1) sys::out::file(dir/"entries_dic.dat") << entries_dic;
            if (up2) sys::out::file(dir/"entries_one.dat") << entries_one;
            if (up3) sys::out::file(dir/"entries_two.dat") << entries_two;
            if (up4) sys::out::file(dir/"media_index.dat") << media_index;
            if (up5) sys::out::file(dir/"media_paths.dat") << media_paths;
            auto f = sys::out::file(dir/"assets.dat");
            f << (int)(assets.size());
            for (auto& asset: assets) {
            f << asset->title;
            f << sys::in::bytes
            (asset->path); }

            if (up1) logs::out << yellow(bold("entries_dic updated"));
            if (up2) logs::out << yellow(bold("entries_one updated"));
            if (up3) logs::out << yellow(bold("entries_two updated"));
            if (up4) logs::out << yellow(bold("media_index updated"));
            if (up5) logs::out << yellow(bold("media_paths updated"));

            logs::out << dark(bold("SAVE OK"));

            report::updated |=
            up1 or up2 or
            up3 or up4 or
            up5;
        }
        catch(std:: exception const& e) {
        logs::err << red(bold(e.what())); }
    };
}
