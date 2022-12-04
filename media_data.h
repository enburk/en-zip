#pragma once
#include <sstream>
#include "medio_audio.h"
#include "medio_video.h"
namespace media::data
{
    namespace out
    {
        struct source : dat::out::file
        {
            struct info { location location; bool used = false; };

            std::map<str, info> content; int number;

            source (std::filesystem::path path, int number) :
                dat::out::file(path, std::ios::binary | std::ios::app),
                number(number)
            {
                std::filesystem::path txt = path;
                txt.replace_extension(".txt");
                if (!std::filesystem::exists(txt)) return;
                array<str> lines = dat::in::text(txt).value();
                for (str line : lines)
                {
                    if (line == "") continue;

                    str offset; line.split_by(" # ", offset, line); offset.strip();
                    str length; line.split_by(" # ", length, line); length.strip();
                    str size_x; line.split_by(" # ", size_x, line); size_x.strip();
                    str size_y; line.split_by(" # ", size_y, line); size_y.strip();
                    str record = line;

                    content [record] = info
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
                            std::string(offset.right_aligned(10)) + " # " +
                            std::string(length.right_aligned(10)) + " # " +
                            std::string(size_x.right_aligned(10)) + " # " +
                            std::string(size_y.right_aligned(10)) + " # " +
                            record + "\n";
                        }
                    }
                    std::filesystem::rename(tmp, txt);
                }
                catch (std::exception & e) {
                    *report::out << bold(red(
                        "media::out::source: " +
                            str(e.what()))); }
                catch (...) {
                    *report::out << bold(red(
                        "media::out::source: "
                        "unknown exception")); }
            }

            expected<Location> add (resource const& r) try
            {
                auto size = std::filesystem::file_size(r.path);
                if (size > (std::uintmax_t)(max<int32_t>()))
                    throw std::out_of_range("media::out::file too big: "
                        + r.path.string());

                auto ftime = std::filesystem::last_write_time(r.path);
                auto xtime = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
                std::time_t ctime = std::chrono::system_clock::to_time_t(xtime);
                std::stringstream stringstream;
                stringstream << std::put_time(std::gmtime(&ctime), "%Y/%m/%d %T");
                str stime = stringstream.str();
                str ssize = std::to_string(size);
                str record = ssize.right_aligned(10) + " # " + stime + " # " + r.id;

                auto it = content.find(record);
                if (it != content.end()) {
                    it->second.used = true;
                    return Location{
                        it->second.location,
                        false}; // old one
                }

                array<byte> data =
                    r.kind == "audio" ? audio::data(r).value():
                    r.kind == "video" ? video::data(r).value():
                    array<byte>{};

                if (data.size() == 0) throw std::runtime_error("no data");
                if (data.size() > max<int32_t>() - dat::out::file::size)
                    return Location{}; // it's enough for this storage

                info info {};
                info.used = true;
                info.location.source = number;
                info.location.offset = dat::out::file::size;
                info.location.length = data.size();

                fstream.write((char*) data.data(), data.size());
                dat::out::file::size += data.size();

                if (r.kind == "video") {
                    auto size = pix::size(data.from(0)).value();
                    info.location.size_x = size.x;
                    info.location.size_y = size.y;
                }

                content[record] = info;

                report::data_updated = true;

                return Location{info.location, true};
            }
            catch (std::exception & e) {
            return aux::error("media::data::out::source:"
                "<br> path: " + r.path.string() +
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

            Location add (const resource & r)
            {
                auto result = sources.back()->add(r).value();
                if (result.location == location{}) { int i = sources.size();
                    std::string filename = "storage." + std::to_string(i) + ".dat";
                    sources += std::make_unique<source>(dir / filename, i);
                    result = sources.back()->add(r).value();
                }
                return result;
            }
        };
    }
}