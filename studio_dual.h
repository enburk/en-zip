#include "app_dual.h"
namespace studio::dual
{
    struct studio : gui::widget<studio>
    {
        gui::canvas canvas;

        studio ()
        {
            canvas.color = gui::skins[skin.now].light.back_color;
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;

                canvas.coord = XYWH(0, 0, W, H);
            }
        }
    };
}

