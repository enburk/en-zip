#pragma once
#include "app.h"
namespace app::one
{
    struct app:
    widget<app>
    {
        app() { reload(); }

        void reload () try
        {
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
            }
        }
    };
}

