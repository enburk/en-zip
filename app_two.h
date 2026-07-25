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
            go(sys::settings::load(
              "app::two::path", ""));
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

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

            stage.fill();
            stage.show_all();
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                stage.coord = coord.now.local();
            }

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

