#pragma once
#include "eng_parser.h"
namespace eng::unittest
{
    bool proceed (vocabulary& vocabulary, gui::console& report)
    {
        bool ok = true;


        report << bold(ok?
        green("UNITTEST OK"):
        red("UNITTEST FAILED")); 
        return ok;
    }
}
