#include "app.h"
namespace app::dic::card
{
    struct card : gui::widget<card>
    {
    };

    struct quot : gui::widget<quot>
    {
    };

    struct area : gui::widget<area>
    {
        gui::area<card> card;
        gui::area<quot> quot;
        gui::canvas tool;
        gui::button undo, redo;

        area ()
        {
            undo.text.text = "undo";
            redo.text.text = "redo";
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int n = (H-10*h) / h;
                int w = W/2;
                int y = 0;

                card.coord = XYWH(0, 0, W, n*h + 2*l); y += card.coord.now.h;
                quot.coord = XYWH(0, y, W, H-y - 2*h); y += quot.coord.now.h;
                tool.coord = XYXY(0, y, W, H);
                undo.coord = XYXY(0, y, w, H);
                redo.coord = XYXY(w, y, W, H);
            }
            if (what == &skin)
            {
                tool.color = gui::skins[skin.now].light.back_color;
            }
        }
    };
}

