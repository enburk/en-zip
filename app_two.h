#pragma once
#include "app.h"
#include "app_two_stage.h"
namespace app::two
{
    struct view:
    widget<view>
    {
        app::two::stage stage;

        property<bool> translated = false;
        property<bool> mute = false;
        property<byte> volume = 255;

        int clicked = 0;
        str where;

        using unit = content::unit;

        void reload () try
        {
            stage.topic = nullptr;
            go(sys::settings::load(
              "app::two::path", ""));
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

        void on (int level, bool on)
        {
            stage.levels[level] = on;
            stage.resize();
        }

        void go (str path)
        {
            stage.go(course.find(path));

            where =
            stage.topic and
            stage.topic->parent?
            stage.topic->parent->path : red(bold(path));
            where.replace_all("/", blue("/"));

            if(
            stage.topic and
            stage.topic->parent) sys::settings::save("app::two::path",
            stage.topic->parent->path);
        }

        void start () { stage.start(); }
        void halt  () { stage.halt (); }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                stage.coord = coord.now.local();
            }

            if (what == &alpha
            and alpha.to  == 255
            and alpha.now == 255)
                stage.start();

            if (what == &alpha
            and alpha.to  == 0
            and alpha.now == 0)
                stage.halt();

            if (what == &stage)
            {
                clicked = stage.clicked;
                notify();
            }

            if (what == &translated)
                stage.translated =
                    translated;

            if (what == &volume)
                stage.volume =
                    volume;

            if (what == &mute)
                stage.mute =
                    mute;
        }
    };
}

