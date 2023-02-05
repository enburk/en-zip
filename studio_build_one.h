#pragma once
#include "eng_parser.h"
#include "media_data.h"
#include "content_course.h"
namespace studio::build::one
{
    void compile
    (
        eng::vocabulary& vocabulary,
        array<int>& redirects,
        media::out::data& data
    )
    {
        content::logs::out = app::logs::report;
        content::logs::err = app::logs::errors;

        content::out::course course("content");

    }
}
