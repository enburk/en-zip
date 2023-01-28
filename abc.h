#pragma once
#include "../auxs/gui_effect.h"
#include "../auxs/gui_colors.h"
#include "../auxs/gui_widget.h"
#include "../auxs/gui_widget_aux.h"
#include "../auxs/gui_widget_image.h"
#include "../auxs/gui_widget_player.h"
#include "../auxs/gui_widget_button.h"
#include "../auxs/gui_widget_scroller.h"
#include "../auxs/gui_widget_text_editline.h"
#include "../auxs/gui_widget_text_editor.h"
#include "../auxs/gui_widget_console.h"
#include "../auxs/doc_html_lexica.h"
#include "../auxs/doc_html_syntax.h"
#include "../auxs/doc_html_utils.h"
#include "../auxs/doc_html_model.h"
#include "../auxs/doc_text_repo.h"
#include "../auxs/aux_unittest.h"
#include "../auxs/pix_process.h"
#include "../auxs/sys_aux.h"

using pix::xy;
using pix::xywh;
using pix::xyxy;
using pix::rgba;
using pix::image;
using pix::frame;
using gui::widget;
using namespace std::literals::chrono_literals;

str bold      (str s) { return "<b>" + s + "</b>"; }
str italic    (str s) { return "<i>" + s + "</i>"; }
str big       (str s) { return "<big>" + s + "</big>"; }
str small     (str s) { return "<small>" + s + "</small>"; }
str blue      (str s) { return "<font color=#000080>" + s + "</font>"; }
str lightblue (str s) { return "<font color=#0000FF>" + s + "</font>"; }
str dark      (str s) { return "<font color=#505050>" + s + "</font>"; }
str gray      (str s) { return "<font color=#808080>" + s + "</font>"; }
str green     (str s) { return "<font color=#008000>" + s + "</font>"; }
str light     (str s) { return "<font color=#A0A0A0>" + s + "</font>"; }
str purple    (str s) { return "<font color=#800080>" + s + "</font>"; }
str red       (str s) { return "<font color=#B00020>" + s + "</font>"; }
str yellow    (str s) { return "<font color=#BF360C>" + s + "</font>"; }
str monospace (str s) { return "<font face=\"monospace\">" + s + "</font>"; }

using namespace std::string_view_literals;
constexpr std::string_view ellipsis = "\xE2" "\x80" "\xA6"sv; // …
constexpr std::string_view mdash    = "\xE2" "\x80" "\x94"sv; // m-dash
constexpr std::string_view ndash    = "\xE2" "\x80" "\x93"sv; // n-dash
constexpr std::string_view lquot    = "\xE2" "\x80" "\x9C"sv; // “
constexpr std::string_view rquot    = "\xE2" "\x80" "\x9D"sv; // ”

str un_msdos (str s)
{
    s.replace_all("~!",  "?");
    s.replace_all('~',1, "/");
    s.replace_all('.',2, ":");
    return s;
}
str to_msdos (str s)
{
    s.replace_all("?", "~!");
    s.replace_all("/", "~" );
    s.replace_all(":", "..");
    return s;
}



