#include "app.h"
#include "app_dic_card.h"
#include "app_dic_list.h"
namespace app::dic
{
    struct app : gui::widget<app>
    {
        card::area card;
        list::area list;

        gui::splitter splitter;

        app()
        {
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int w = 25*h; // list width
                int d = 2*l;

                splitter.lower =   500 * W / 10'000;
                splitter.upper = 5'000 * W / 10'000;

                int p = sys::settings::load("app::dic::splitter.permyriad", 10'000 * (W-w)/W);
                p = aux::clamp<int>(p, splitter.lower.now, splitter.upper.now);
                w = W * p / 10'000;

                splitter.coord = XYWH(W-w-d, 0, 2*d, H);

                card.coord = XYWH(0, 0, W-w, H);
                list.coord = XYWH(W-w, 0, w, H);
            }
        }

        void on_focus (bool on) override { list.on_focus(on); }
        void on_keyboard_input (str symbol) override { list.on_keyboard_input(symbol); }
        void on_key_pressed (str key, bool down) override { list.on_key_pressed(key,down); }

        void on_notify (gui::base::widget* w) override
        {
            if (w == &card)
            {
            }
            if (w == &list)
            {
            }
        }

        void on_notify (gui::base::widget* w, int n) override
        {
            if (w == &splitter) {
                sys::settings::save("app::dic::splitter.permyriad",
                10'000 * n / coord.now.w);
                on_change(&coord);
            }
        }
    };
}

