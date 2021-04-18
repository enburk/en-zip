#pragma once
#include <sstream>
#include "media_audio.h"
#include "media_video.h"
namespace media::data
{
    struct location
    {
        int32_t source = 0;
        int32_t offset = 0;
        int32_t length = 0;
        int32_t size_x = 0;
        int32_t size_y = 0;

        bool operator == (location const&) const = default;
        bool operator != (location const&) const = default;

        friend void operator >> (dat::in::pool & in, location & l) {
            l.source = in.get_int();
            l.offset = in.get_int();
            l.length = in.get_int();
            l.size_x = in.get_int();
            l.size_y = in.get_int();
        }
        friend void operator << (dat::out::pool & out, location & l) {
            out << l.source;
            out << l.offset;
            out << l.length;
            out << l.size_x;
            out << l.size_y;
        }
    };

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
                    *report::out << "<b><font color=#B00020>"
                        "media::out::source: " + str(e.what()) +
                        "</font></b>";
                }
                catch (...) {
                    *report::out << "<b><font color=#B00020>"
                        "media::out::source: unknown exception"
                        "</font></b>";
                }
           }

            expected<location> add (const resource & r) try
            {
                auto size = std::filesystem::file_size(r.path);
                if (size > (std::uintmax_t)(max<int32_t>()))
                    throw std::out_of_range("media::out::file too big: "
                        + r.path.string());

                auto time = std::filesystem::last_write_time(r.path);
                // std::chrono::clock_cast<std::chrono::system_clock>(time);
                // next code sometimes give one second error
                // and causes unnecessary duplication:
                auto sctp = std::chrono::time_point_cast<
                    std::chrono::system_clock::duration>(
                        time - decltype(time)::clock::now()
                        + std::chrono::system_clock::now());
                std::time_t ctime = std::chrono::system_clock::to_time_t(sctp);
                std::stringstream stringstream;
                stringstream << std::put_time(std::gmtime(&ctime), "%Y/%m/%d %T");
                str stime = stringstream.str();
                str ssize = std::to_string(size);

                str record = ssize.right_aligned(10) + " # " + stime + " # " + r.id;

                auto it = content.find(record);
                if (it != content.end()) {
                    it->second.used = true;
                    return it->second.location;
                }

                array<byte> data =
                    r.kind == "audio" ? audio::data(r).value():
                    r.kind == "video" ? video::data(r).value():
                    array<byte>{};

                if (data.size() == 0) throw std::runtime_error("no data");
                if (data.size() > max<int32_t>() - dat::out::file::size)
                    return location{}; // it's enough

                info info {};
                info.used = true;
                info.location.source = number;
                info.location.offset = dat::out::file::size;
                info.location.length = data.size();

                if (true) *report::out << 
                "<font color=#800080>" + r.title + "</font>" +
                "<font color=#000080>" + "[" + str(r.entries, "][") + "]" + "</font>" +
                "<font color=#008000>" + "{" + str(r.options, "}{") + "}" + "</font>" ;

                if (false) {
                *report::out << r.id;
                *report::out << "location.source: " + std::to_string(info.location.source);
                *report::out << "location.offset: " + std::to_string(info.location.offset);
                *report::out << "location.length: " + std::to_string(info.location.length);
                }

                fstream.write((char*) data.data(), data.size());
                dat::out::file::size += data.size();

                if (r.kind == "video") {
                    auto size = pix::size(data.from(0)).value();
                    info.location.size_x = size.x;
                    info.location.size_y = size.y;
                }

                content[record] = info;

                ::media::report::data_updated = true;

                return info.location;
            }
            catch (std::exception & e) {
            return ::data::error("media::data::out::source:"
                "<br> path: " + r.path.string() +
                "<br> " + e.what());
            }
        };

        struct storage
        {
            array<std::unique_ptr<source>> sources; path dir;

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

            location add (const resource & r)
            {
                auto result = sources.back()->add(r).value();
                if (result == location{}) { int i = sources.size();
                    std::string filename = "storage." + std::to_string(i) + ".dat";
                    sources += std::make_unique<source>(dir / filename, i);
                    result = sources.back()->add(r).value();
                }
                return result;
            }
        };
    }

    struct index
    {
        int32_t entry; array<location> links;
    };
}