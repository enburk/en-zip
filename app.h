#pragma once
#include "../auxs/gui_widget_button.h"
#include "../auxs/gui_widget_console.h"
#include "../auxs/gui_widget_splitter.h"
#include "../auxs/sfx_medio_sequencer.h"
#include "../auxs/sfx_media_vudio.h"
#include "content_course.h"
#include "eng_parser.h"
#include "media_data.h"

using gui::time;
using gui::widget;
using gui::widgetarium;
using gui::unary_property;
using gui::binary_property;
using gui::property;

namespace app
{
    eng::vocabulary vocabulary;
    namespace one { content::in::course course; }
    namespace two { content::in::course course; }
    namespace logs
    {
        optional_log times;
        optional_log media;
        optional_log audio;
        optional_log video;
        optional_log report;
        optional_log errors;
    }

    struct appdatatype
    {
        str error;
        appdatatype () { reload(); }
        void reload () try
        {
            error = "";

            timing t0;
            vocabulary =
            eng::vocabulary("../data/"
                "vocabulary.dat");

            timing t1;
            one::course.load("../data/course.dat", "../data/course_enties.dat");
            two::course.load("../data/catalogs.dat", "../data/catalogs_entires.dat");

            timing t2;
            mediadata.reload();

            timing t3;
            logs::report << gray(monospace(
            "app vocabulary  " + format(t1-t0) + " sec<br>" +
            "app load course " + format(t2-t1) + " sec<br>" +
            "app load media  " + format(t3-t2) + " sec<br>" +
            "app load total  " + format(t3-t0) + " sec<br>"));
        }
        catch (std::exception const& e) {
        error = bold(red(e.what())); }
    };
    appdatatype
    appdata;
}
