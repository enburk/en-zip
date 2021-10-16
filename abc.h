#pragma once
#include "../ae/proto-studio/gui_effect.h"
#include "../ae/proto-studio/gui_colors.h"
#include "../ae/proto-studio/gui_widget.h"
#include "../ae/proto-studio/gui_widget_image.h"
#include "../ae/proto-studio/gui_widget_player.h"
#include "../ae/proto-studio/gui_widget_canvas.h"
#include "../ae/proto-studio/gui_widget_button.h"
#include "../ae/proto-studio/gui_widget_scroller.h"
#include "../ae/proto-studio/gui_widget_text_editor1.h"
#include "../ae/proto-studio/gui_widget_text_editor.h"
#include "../ae/proto-studio/gui_widget_console.h"
#include "../ae/proto-studio/doc_html_lexica.h"
#include "../ae/proto-studio/doc_html_syntax.h"
#include "../ae/proto-studio/doc_html_utils.h"
#include "../ae/proto-studio/doc_html_model.h"
#include "../ae/proto-studio/pix_process.h"
#include "../ae/proto-studio/sys_aux.h"

using pix::XY;
using pix::XYWH;
using pix::XYXY;
using pix::RGBA;
using pix::image;
using pix::frame;
using gui::widget;

str bold   (str s) { return "<b>" + s + "</b>"; }
str italic (str s) { return "<i>" + s + "</i>"; }
str blue   (str s) { return "<font color=#000080>" + s + "</font>"; }
str dark   (str s) { return "<font color=#505050>" + s + "</font>"; }
str gray   (str s) { return "<font color=#808080>" + s + "</font>"; }
str green  (str s) { return "<font color=#008000>" + s + "</font>"; }
str light  (str s) { return "<font color=#A0A0A0>" + s + "</font>"; }
str purple (str s) { return "<font color=#800080>" + s + "</font>"; }
str red    (str s) { return "<font color=#B00020>" + s + "</font>"; }
str yellow (str s) { return "<font color=#BF360C>" + s + "</font>"; }

using namespace std::string_view_literals;
constexpr std::string_view ellipsis = "\xE2" "\x80" "\xA6"sv; // …
constexpr std::string_view mdash    = "\xE2" "\x80" "\x94"sv; // m-dash
constexpr std::string_view ndash    = "\xE2" "\x80" "\x93"sv; // n-dash
constexpr std::string_view lquot    = "\xE2" "\x80" "\x9C"sv; // “
constexpr std::string_view rquot    = "\xE2" "\x80" "\x9D"sv; // ”



