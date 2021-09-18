#pragma once

#include "media.h"
#include "medio_audio.h"
#include "medio_video.h"
#include "eng_dictionary.h"

inline auto & vocabulary = eng::vocabulary::data;

inline std::map<str, array<sys::byte>> assets;

namespace app::dic { inline gui::console* log = nullptr; }
namespace app::one { inline gui::console* log = nullptr; }
namespace app::two { inline gui::console* log = nullptr; }




