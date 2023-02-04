#pragma once

#include "media_data.h"
#include "eng_parser.h"

struct optional_log
{
    optional_log() = default;
    optional_log(gui::console& log) : log(&log) {}
    void operator << (str s) { if (log) *log << std::move(s); }
    void clear () { if (log) log->clear(); }
    private: gui::console* log = nullptr;
};

namespace app::logs
{
    optional_log times;
    optional_log media;
    optional_log audio;
    optional_log video;
    optional_log errors;
}
namespace app
{
    media::in::data mediadata;
    auto & assets = mediadata.assets;
    eng::vocabulary vocabulary;

    struct appdatatype
    {
        appdatatype () { reload(); }
        void reload () try
        {
            timing t0;
            vocabulary =
            eng::vocabulary("../data/"
                "vocabulary.dat");

            timing t1;
            mediadata.reload();

            timing t2;
            logs::times << gray(monospace(
            "app vocabulary  " + format(t1-t0) + " sec<br>" +
            "app load media  " + format(t2-t1) + " sec<br>" +
            "app load total  " + format(t2-t0) + " sec<br>"));
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }
    };
    appdatatype appdata;
}
