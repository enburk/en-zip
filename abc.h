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
#include "../ae/proto-studio/doc_html_syntax.h"
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
str green  (str s) { return "<font color=#008000>" + s + "</font>"; }
str yellow (str s) { return "<font color=#BF360C>" + s + "</font>"; }
str red    (str s) { return "<font color=#B00020>" + s + "</font>"; }
str purple (str s) { return "<font color=#800080>" + s + "</font>"; }




