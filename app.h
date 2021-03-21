#pragma once
#include "../ae/proto-studio/gui_effect.h"
#include "../ae/proto-studio/gui_colors.h"
#include "../ae/proto-studio/gui_widget.h"
#include "../ae/proto-studio/gui_widget_image.h"
#include "../ae/proto-studio/gui_widget_canvas.h"
#include "../ae/proto-studio/gui_widget_button.h"
#include "../ae/proto-studio/gui_widget_scroller.h"
#include "../ae/proto-studio/gui_widget_text_editor1.h"
#include "../ae/proto-studio/gui_widget_text_editor.h"
#include "../ae/proto-studio/gui_widget_console.h"
#include "../ae/proto-studio/doc_html_syntax.h"
#include "../ae/proto-studio/doc_html_model.h"
using pix::XY;
using pix::XYWH;
using pix::XYXY;
using pix::RGBA;
using pix::image;
using pix::frame;

#include "media_data.h"

inline auto & vocabulary = eng::vocabulary::data;

namespace app::dict { inline gui::console* log = nullptr; }
namespace app::dual { inline gui::console* log = nullptr; }
namespace app::mono { inline gui::console* log = nullptr; }





