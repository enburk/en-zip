#include "app_pix.h"
namespace studio::pix
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

