#pragma once
#include "eng_parser.h"
#include "content_course.h"
#include "media_data.h"

namespace app
{
    eng::vocabulary vocabulary;
    media::in::data mediadata;
    auto & assets = mediadata.assets;
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
            one::course.load("../data");
            two::course.load("../data");

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
