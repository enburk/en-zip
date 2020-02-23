#include "app_rus.h"
namespace studio::rus
{
    struct studio : gui::widget<studio>
    {
        gui::canvas canvas;

        studio ()
        {
            canvas.color = gui::skins[""].light.back_color;
        }
    };
}

