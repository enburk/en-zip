#pragma once
#include "app_dic_left.h"
#include "app_dic_list.h"
namespace app::dic
{
    struct app : gui::widget<app>
    {
        left::area left;
        list::area list;

        gui::splitter splitter;

        app() { reload(); }

        void reload ()
        {
            dat::in::pool pool;
            std::filesystem::path dir = "../data";
            if (!std::filesystem::exists(dir / "vocabulary.dat")) return;
            pool.bytes = dat::in::bytes (dir / "vocabulary.dat").value();
            dat::in::endianness = 0; // otherwise it would be reversed
            dat::in::endianness = pool.get_int();
            vocabulary.resize(pool.get_int());
            for (auto & entry : vocabulary)
                pool >> entry;

            assets.clear();
            if (std::filesystem::exists    (dir / "app_dict" / "assets.dat")) {
                pool.bytes = dat::in::bytes(dir / "app_dict" / "assets.dat").value();
                pool.offset = 0;
                int nn = pool.get_int();
                for (int i=0; i<nn; i++) {
                    auto title = pool.get_string();
                    auto bytes = pool.get_bytes();
                    assets[title] = std::vector<sys::byte>(
                        bytes.first,
                        bytes.first +
                        bytes.second);
                }
            }

            media::reload();
            left.current = eng::vocabulary::entry{};
            left.reload();
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
                int x = clamp<int>(W*p / 100'00, splitter.lower.now, splitter.upper.now);

                splitter.coord = XYXY(x-d, 0, x+d, H);

                left.coord = XYXY(0, 0, x, H);
                list.coord = XYXY(x, 0, W, H);
            }
        }

        void on_focus (bool on) override { list.on_focus(on); }
        void on_keyboard_input (str symbol) override
        {
            left.card.object.text.view.selections = array<gui::text::range>();
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
                left.card.object.text.view.selected() != "") {
                left.card.object.text.on_key_pressed(key,down);
                return;
            }

            left.card.object.text.view.selections = array<gui::text::range>();
            list.on_key_pressed(key,down);
        }

        void on_notify (void* what) override
        {
            if (what == &left) list.select(left.clicked);
            if (what == &list) left.select(list.clicked);
            if (what == &splitter) {
                sys::settings::save(
                "app::dic::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }
        }
    };
}

