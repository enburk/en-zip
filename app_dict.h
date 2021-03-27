#pragma once
#include "app_dict_card.h"
#include "app_dict_list.h"
namespace app::dict
{
    struct app : gui::widget<app>
    {
        card::area card;
        list::area list;

        gui::splitter splitter;

        app() { reload(); }

        void reload ()
        {
            dat::in::pool pool;
            std::filesystem::path dir = "../data";
            if (!std::filesystem::exists (dir / "vocabulary.dat")) return;
            pool.bytes = dat::in::bytes(dir / "vocabulary.dat").value();
            dat::in::endianness = 0; // otherwise it would be reversed
            dat::in::endianness = pool.get_int();
            vocabulary.resize(pool.get_int());
            for (auto & entry : vocabulary)
                pool >> entry;

            mediae::reload();

            card.current = eng::vocabulary::entry{};
            list.reload();
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int w = 17*h; // list width
                int d = 2*l;

                splitter.lower = W * 50'00 / 100'00;
                splitter.upper = W * 90'00 / 100'00;

                int p = sys::settings::load("app::dic::splitter.permyriad", 100'00 * (W-w)/W);
                int x = data::clamp<int>(W*p / 100'00, splitter.lower.now, splitter.upper.now);

                splitter.coord = XYXY(x-d, 0, x+d, H);

                card.coord = XYXY(0, 0, x, H);
                list.coord = XYXY(x, 0, W, H);
            }
        }

        void on_focus (bool on) override { list.on_focus(on); }
        void on_keyboard_input (str symbol) override
        {
            card.card.object.text.view.selections = array<gui::text::range>();
            list.on_keyboard_input(symbol);
        }
        void on_key_pressed (str key, bool down) override
        {
            if (key == "") return;
            if((key == "ctrl+insert" or
                key == "shift+left"  or
                key == "shift+right" or
                key == "ctrl+left"   or
                key == "ctrl+right"  or
                key == "ctrl+shift+left"  or
                key == "ctrl+shift+right" or
                key == "shift+up"    or
                key == "shift+down") and
                card.card.object.text.view.selected() != "") {
                card.card.object.text.on_key_pressed(key,down);
                return;
            }

            card.card.object.text.view.selections = array<gui::text::range>();
            list.on_key_pressed(key,down);
        }

        void on_notify (void* what) override
        {
            if (what == &card) list.select(card.clicked);
            if (what == &list) card.select(list.clicked);
            if (what == &splitter) {
                sys::settings::save(
                "app::dic::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }
        }
    };
}

