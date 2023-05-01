#pragma once
#include <sstream>
#include "medio_audio.h"
#include "medio_video.h"
namespace media::out
{
    struct source : sys::out::file
    {
        struct info { location location; bool used = false; };

        std::map<str, info> content; int number;

        source (std::filesystem::path path, int number) :
            sys::out::file(path, std::ios::binary | std::ios::app),
            number(number)
        {
            std::filesystem::path txt = path;
            txt.replace_extension(".txt");
            if (!std::filesystem::exists(txt)) return;
            for (str text = sys::in::text(txt); str line: text.lines())
            {
                line.strip(); if (line == "") continue;

                str offset = line.extract_upto(" # ");
                str length = line.extract_upto(" # ");
                str size_x = line.extract_upto(" # ");
                str size_y = line.extract_upto(" # ");
                str record = line;

                content[record] = info
                {
                    location {number,
                    std::stoi(offset),
                    std::stoi(length),
                    std::stoi(size_x),
                    std::stoi(size_y)},
                    false
                };
            }
        }

        ~source ()
        {
            try
            {
                if (content.size() == 0) return;
                std::filesystem::path txt = path; txt.replace_extension(".txt");
                std::filesystem::path tmp = path; tmp.replace_extension(".tmp");
                {
                    auto dir = tmp.parent_path();
                    if (dir != std::filesystem::path())
                        std::filesystem::create_directories(dir);

                    std::ofstream fstream(tmp);
                    for (auto [record, info] : content)
                    {
                        str offset = std::to_string(info.location.offset);
                        str length = std::to_string(info.location.length);
                        str size_x = std::to_string(info.location.size_x);
                        str size_y = std::to_string(info.location.size_y);
                        fstream <<
                        offset.right_aligned(10) + " # " +
                        length.right_aligned(10) + " # " +
                        size_x.right_aligned(10) + " # " +
                        size_y.right_aligned(10) + " # " +
                        record + "\n";
                    }
                }
                std::filesystem::rename(tmp, txt);
            }
            catch (std::exception & e) {
                logs::out << bold(red(
                    "media::out::source: " +
                        str(e.what()))); }
            catch (...) {
                logs::out << bold(red(
                    "media::out::source: "
                    "unknown exception")); }
        }

        expected<Location> add (resource const& r, int app) try
        {
            auto size = std::filesystem::file_size(r.path);
            if (size > (std::uintmax_t)(max<int32_t>()))
                throw std::out_of_range(
                "media: file too big: "
                + str(r.path));

            auto ftime = std::filesystem::last_write_time(r.path);
            auto xtime = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
            std::time_t ctime = std::chrono::system_clock::to_time_t(xtime);

            std::stringstream stringstream;
            stringstream << std::put_time(
            std::gmtime(&ctime), "%Y/%m/%d %T");

            str stime = stringstream.str();
            str ssize = std::to_string(size);
            str record = ssize + " # " + stime + " # " + r.id;

            str cropkind = app == 0 ? "crop" : "qrop";
            if (cropkind == "qrop" and r.opt("qrop") != "")
                record += " (qrop)";

            auto it = content.find(record);
            if (it != content.end()) {
                it->second.used = true;
                return Location{
                    it->second.location,
                    false}; // old one
            }

            array<byte> data =
                r.kind == "audio" ? audio::data(r, cropkind).value():
                r.kind == "video" ? video::data(r, cropkind).value():
                array<byte>{};

            if (data.size() == 0) throw std::runtime_error("no data");
            if (data.size() > max<int32_t>() - sys::out::file::size)
                return Location{}; // it's enough for this storage

            info info {};
            info.used = true;
            info.location.source = number;
            info.location.offset = sys::out::file::size;
            info.location.length = data.size();

            fstream.write((char*) data.data(), data.size());
            sys::out::file::size += data.size();

            if (r.kind == "video") {
                auto size = pix::size(data.from(0)).value();
                info.location.size_x = size.x;
                info.location.size_y = size.y;
            }

            content[record] = info;

            report::updated = true;

            return Location{info.location, true};
        }
        catch (std::exception & e) {
        return aux::error("media::data::out::source:"
            "<br> path: " + str(r.path) +
            "<br> " + e.what());
        }
    };

    struct storage
    {
        path dir;

        array<std::unique_ptr<source>> sources;
            
        storage (path dir) : dir(dir)
        {
            for (int i=0; i<1000; i++)
            {
                std::string filename = "storage." + std::to_string(i) + ".dat";
                if (!std::filesystem::exists(dir / filename)) break;
                sources += std::make_unique<source>(dir / filename, i);
            }
            if (sources.size() == 0)
                sources += std::make_unique<source>(dir / "storage.0.dat", 0);
        }

        Location add (const resource & r, int app)
        {
            auto result = sources.back()->add(r, app).value();
            if (result.location == location{}) { int i = sources.size();
                std::string filename = "storage." + std::to_string(i) + ".dat";
                sources += std::make_unique<source>(dir / filename, i);
                result = sources.back()->add(r, app).value();
            }
            return result;
        }
    };
}
