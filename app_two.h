#pragma once
#include "app.h"
namespace app::two
{
    struct view:
    widget<view>
    {
        property<bool> translated = false;

        int clicked = 0;

        using unit = content::unit;

        void reload () try
        {
            go(sys::settings::load(
              "app::two::path", ""));
        }
        catch (std::exception const& e) {
            logs::errors << bold(red(
                e.what())); }

        void go (str)
        {
        }
    };
}

