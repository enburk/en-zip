#pragma once
#include "app.h"
#include "app_one_slide.h"
namespace app::one
{
    struct stage:
    widget<stage>
    {
        widgetarium<slide> slides;
        widgetarium<entry> entries;
    };
}