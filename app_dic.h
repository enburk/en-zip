#pragma once
#include "app_dic_left.h"
#include "app_dic_list.h"
namespace app::dic
{
    struct app:
    widget<app>
    {
        left::area left;
        list::area list;

        gui::splitter splitter;

        app() { reload(); }

        void reload () try
        {
            timing t0;
            std::filesystem::path dir = "../data";

            vocabulary = std::move(::eng::vocabulary(
                    dir/"vocabulary.dat"));

            timing t1;
            assets.clear();
            dat::in::pool pool(dir/"app_dict"/"assets.dat");
            int nn = pool.get_int();
            for (int i=0; i<nn; i++) {
                auto title = pool.get_string();
                auto bytes = pool.get_bytes();
                assets[title] = std::vector<sys::byte>(
                    bytes.data,
                    bytes.data +
                    bytes.size);
            }

            timing t2;
            media::reload();
            left.current_entry = ::eng::dictionary::entry{};
            left.current_index = ::eng::dictionary::index{};
            left.reload();
            list.reload();

            timing t3;
            logs::times << gray(monospace(
            "app vocabulary  " + format(t1-t0) + " sec<br>" +
            "app load assets " + format(t2-t1) + " sec<br>" +
            "app load media  " + format(t3-t2) + " sec<br>" +
            "app load total  " + format(t3-t0) + " sec<br>"));
        }
        catch (std::exception & e) {
            logs::times << bold(red(
                e.what())); }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int w = 10*h; // list width
                int d = 2*l;

                splitter.lower = W * 50'00 / 100'00;
                splitter.upper = W * 90'00 / 100'00;

                str s = "app::dic::splitter.permyriad";
                int p = sys::settings::load(s, 100'00 * (W-w)/W);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower.now,
                splitter.upper.now);

                splitter.coord = xyxy(x-d, 0, x+d, H);

                left.coord = xyxy(0, 0, x, H);
                list.coord = xyxy(x, 0, W, H);
            }

            if (what == &left) list.select(left.clicked);
            if (what == &list) left.select(list.clicked);
            if (what == &splitter) {
                sys::settings::save(
                "app::dic::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }
        }

        void on_key(str key, bool down, bool input) override
        {
            if (input
            or  key == "left"
            or  key == "right"
            or  key == "enter"
            or  key == "escape"
            or  key == "delete"
            or  key == "backspace"
            or  key == "shift+insert"
            or  key == "ctrl+V")
                focus = &list;

            if (focus.now)
                focus.now->on_key(key, down, input);
        }
    };
}

