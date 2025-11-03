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
using std::filesystem::path;

namespace app
{
    double speed = 1.0;

    eng::vocabulary vocabulary;
    eng::dictionary dictionary;
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

    struct dicdatatype
    {
        str error;
        str report;
        dicdatatype () { reload(); }
        void reload () try
        {
            error = "";
            static int n = 0;
            report = gray(monospace(
            "dic data loading " +
            std::to_string(++n) +
                "...<br>"));

            timing t0;
            vocabulary = eng::vocabulary("../data/");

            timing t1;
            dictionary = eng::dictionary("../data/",
            vocabulary.size());

            timing t2;
            report += gray(monospace(
            "dic vocabulary " + format(t1-t0) + " sec<br>" +
            "dic dictionary " + format(t2-t1) + " sec<br>" +
            "dic load total " + format(t2-t0) + " sec<br>"));
        }
        catch (std::exception const& e) {
        error = bold(red(e.what())); }
    };
    dicdatatype
    dicdata;

    struct appdatatype
    {
        str error;
        str report;
        array<eng::dictionary::index> dictionary_index;


        appdatatype () { reload(); }
        void reload () try
        {
            error = "";
            static int n = 0;
            report = gray(monospace(
            "app data loading " +
            std::to_string(++n) +
                "...<br>"));

            timing t0;
            one::course.load("../data/course.dat", "../data/course_entries.dat");
            two::course.load("../data/catalog.dat", "../data/catalog_entries.dat");
            one::course.root.shuffle();

            timing t1;
            mediadata.reload();

            timing t2;
            report += gray(monospace(
            "app load course " + format(t1-t0) + " sec<br>" +
            "app load media  " + format(t2-t1) + " sec<br>" +
            "app load total  " + format(t2-t0) + " sec<br>"));
        }
        catch (std::exception const& e) {
        error = bold(red(e.what())); }
    };
    appdatatype
    appdata;

    struct lasts
    {
        gui::time time;
        std::deque<str> queue;

        void add (str id)
        {
            time = gui::time::now;
            queue.push_front(id);
            if (queue.size() > 10)
            queue.pop_back();
        }

        int num (str id)
        {
            int num = 0;
            for (str& i: queue)
            if (id == i)
            return num;
            else ++num;
            return num;
        }
    };
    lasts last_vocals;
    lasts last_sounds;
    lasts last_videos;
}
