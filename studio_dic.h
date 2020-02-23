#include "app_dic.h"
#include "studio_dic_update.h"
namespace studio::dic
{
    struct area : gui::widget<area>
    {
    };

    struct studio : gui::widget<studio>
    {
        gui::area<area> area;
        app::dic::app app;
        gui::splitter splitter;

        studio ()
        {
            if (wiktionary_update())
            {
            }
        }


        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int w = W/2;
                int d = 2*l;

                splitter.coord = XYWH(W-w-d, 0, 2*d, H);
                splitter.lower = 8'000 * W / 10'000;
                splitter.upper =   800 * W / 10'000;

                area.coord = XYWH(0, 0, W-w, H);
                app .coord = XYWH(W-w, 0, w, H);
            }
        }
    };
}
