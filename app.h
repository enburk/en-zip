#pragma once

#include "media_data.h"
#include "eng_parser.h"

struct optional_log
{
    optional_log() = default;
    optional_log(gui::console& log) : log(&log) {}
    void clear () { if (log) log->clear(); }
    void operator << (str s) { if (log) *log << std::move(s); }
    private: gui::console* log = nullptr;
};

::media::in::data mediadata;

auto& assets = mediadata.assets;

namespace app::dic
{
    eng::vocabulary vocabulary;

    namespace logs
    {
        optional_log times;
        optional_log media;
        optional_log audio;
        optional_log video;
    }
}

namespace app::one { optional_log log; }
namespace app::two { optional_log log; }
