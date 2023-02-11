#pragma once
#include "media_scan.h"
#include "media_storage.h"
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
        int add (resource* r)
        {
            if (r->index != -1) return
                r->index; else
                r->index = media_index.size();

            auto[location, new_one] = storage.add(*r);

            if (new_one) {
                new_ones.insert(r);
                logs::out << html(
                r->path.string()); }

            ::media
            ::media_index m;
            m.kind     = r->kind;
            m.title    = r->title;
            m.sense    = r->sense;
            m.comment  = r->comment;
            m.credit   = r->credit;
            m.options  = r->options;
            m.location = location;
            media_index += m;
            media_paths +=
            r->path.string();
            return r->index;
        }
        void dic_add(int entry, resource* r) { entries_dic.emplace_back(entry, add(r)); }
        void one_add(int entry, resource* r) { entries_one.emplace_back(entry, add(r)); }
        void two_add(int entry, resource* r) { entries_two.emplace_back(entry, add(r)); }
        ~data ()
        { try {

            logs::out << dark(bold("SAVE..."));

            std::ranges::stable_sort(entries_dic, {}, &::media::entry_index::entry);
            std::ranges::stable_sort(entries_one, {}, &::media::entry_index::entry);
            std::ranges::stable_sort(entries_two, {}, &::media::entry_index::entry);

            path dir = "../data/media";
            auto
            f = sys::out::file(dir/"entries_dic.dat"); f << entries_dic;
            f = sys::out::file(dir/"entries_one.dat"); f << entries_one;
            f = sys::out::file(dir/"entries_two.dat"); f << entries_two;
            f = sys::out::file(dir/"media_index.dat"); f << media_index;
            f = sys::out::file(dir/"media_paths.dat"); f << media_paths;
            f = sys::out::file(dir/"assets.dat");
            f << (int)(assets.size());
            for (auto& asset: assets) {
            f << asset->title;
            f << sys::in::bytes
                (asset->path)
                .value(); }

            logs::out << dark(bold("SAVE OK"));
        }
        catch(std:: exception const& e) {
        logs::err << red(bold(
            e.what())); }}
    };
}
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