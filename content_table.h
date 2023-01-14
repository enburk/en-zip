#pragma once
#include "content_topic.h"
namespace content
{
    struct table
    {
        struct name { int order; str title; };

        void shuffle()
        {
            std::random_device rd;
            std::mt19937 g(rd());
 
            //std::shuffle(v.begin(), v.end(), g);
        }
    };
}
