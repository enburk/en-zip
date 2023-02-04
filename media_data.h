#pragma once
#include "media_scan.h"
#include "media_storage.h"
namespace media::out
{
    struct data
    {
        storage storage;
        dat::out::file entries_dic;
        dat::out::file entries_one;
        dat::out::file entries_two;
        dat::out::file media_index;
        dat::out::file locationary;
        dat::out::file assets_data;
        
        array<resource> resources;
        std::set<resource const*> new_ones;
        std::set<resource const*> assets;
        std::multimap<int, int> entrymap_dic;
        std::multimap<int, int> entrymap_one;
        std::multimap<int, int> entrymap_two;
        std::map<int32_t, std::map<int32_t, str>> locations;
        int total_media = 0;

        data (gui::console& out, gui::console& err):

            storage    ("../data/media"),
            entries_dic("../data/media/entries_dic.dat"),
            entries_one("../data/media/entries_one.dat"),
            entries_two("../data/media/entries_two.dat"),
            media_index("../data/media/media_index.dat"),
            locationary("../data/media/locationary.dat"),
            assets_data("../data/media/assets.dat")
        {
            setlocale(LC_ALL,"en_US.utf8");

            report::out = &out;
            report::err = &err;
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
        void add (resource const* r)
        {
            auto[location, new_one] = storage.add(*r);

            if (new_one) {
                new_ones.insert(r);
                *report::out <<
                doc::html::encoded(
                r->path.string()); }

            locations
            [location.source]
            [location.offset] =
                r->path.string();

            media_index << r->kind;
            media_index << r->title;
            media_index << r->sense;
            media_index << r->comment;
            media_index << r->credit;
            media_index << r->options;
            media_index << location;
            total_media++;
        }
        ~data ()
        { try {

            *report::out << dark(bold("SAVE..."));

            entries_dic << total_media;
            entries_dic << (int)(entrymap_dic.size());
            for (auto [entry, media] : entrymap_dic) {
            entries_dic << entry;
            entries_dic << media; }
        
            entries_one << (int)(entrymap_one.size());
            for (auto [entry, media] : entrymap_one) {
            entries_one << entry;
            entries_one << media; }
        
            entries_two << (int)(entrymap_two.size());
            for (auto [entry, media] : entrymap_two) {
            entries_two << entry;
            entries_two << media; }
        
            assets_data << (int)(assets.size());
            for (auto& resource: assets) {
            assets_data << resource->title;
            assets_data << dat::in::bytes
            (resource->path).value(); }

            for (auto& [source, map] : locations)
            for (auto& [offset, path] : map) {
            locationary << source;
            locationary << offset;
            locationary << path; }

            *report::out << dark(bold("SAVE OK"));
        }
        catch(std:: exception const& e) {
        *report::err << red(bold(
            e.what())); }}
    };
}
namespace media::in
{
    struct entry_index { int32_t entry, media; };
    struct media_index
    {
        str kind;
        str title, sense;
        str comment, credit;
        location location;
        array<str> options;

        bool operator == (media_index const&) const = default;
        bool operator != (media_index const&) const = default;
    };

    struct data
    {
        array<entry_index> entries_dic;
        array<entry_index> entries_one;
        array<entry_index> entries_two;
        array<media_index> media_index;
        std::map<str, array<sys::byte>> assets;
        std::map<int, std::map<int, str>> locations;

        void reload ()
        {
            entries_dic.clear();
            entries_one.clear();
            entries_two.clear();
            media_index.clear();
            assets.clear();

            std::filesystem::path dir = "../data/media";

            dat::in::pool pool;
            if (std::filesystem::exists(dir/"entries_dic.dat")) {
            pool.bytes = dat::in::bytes(dir/"entries_dic.dat").value();
            media_index.resize(pool.get_int());
            entries_dic.resize(pool.get_int());
            for (auto & index : entries_dic) {
                index.entry = pool.get_int();
                index.media = pool.get_int();
            }}
            pool.offset = 0; // reuse
            if (std::filesystem::exists(dir/"entries_one.dat")) {
            pool.bytes = dat::in::bytes(dir/"entries_one.dat").value();
            entries_one.resize(pool.get_int());
            for (auto & index : entries_one) {
                index.entry = pool.get_int();
                index.media = pool.get_int();
            }}
            pool.offset = 0; // reuse
            if (std::filesystem::exists(dir/"entries_two.dat")) {
            pool.bytes = dat::in::bytes(dir/"entries_two.dat").value();
            entries_two.resize(pool.get_int());
            for (auto & index : entries_two) {
                index.entry = pool.get_int();
                index.media = pool.get_int();
            }}
            pool.offset = 0; // reuse
            if (std::filesystem::exists(dir/"media_index.dat")) {
            pool.bytes = dat::in::bytes(dir/"media_index.dat").value();
            for (auto & index : media_index) {
                index.kind    = pool.get_string();
                index.title   = pool.get_string();
                index.sense   = pool.get_string();
                index.comment = pool.get_string();
                index.credit  = pool.get_string();
                index.options.resize(pool.get_int());
                for (auto & option : index.options)
                option = pool.get_string();
                pool >> index.location;
            }}
            pool.offset = 0; // reuse
            if (std::filesystem::exists(dir/"assets.dat")) {
            pool.bytes = dat::in::bytes(dir/"assets.dat").value();
            int nn = pool.get_int();
            for (int i=0; i<nn; i++) {
                auto title = pool.get_string();
                auto bytes = pool.get_bytes();
                assets[title] = std::vector<sys::byte>(
                    bytes.data,
                    bytes.data +
                    bytes.size);
            }}
            pool.offset = 0; // reuse
            if (std::filesystem::exists(dir/"locationary.dat")) {
            pool.bytes = dat::in::bytes(dir/"locationary.dat").value();
            while (not pool.done()) {
                int source = pool.get_int();
                int offset = pool.get_int();
                str path = pool.get_string();
                locations[source][offset] = path;
            }}
        }
    };
}