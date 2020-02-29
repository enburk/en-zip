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
            reload();
        }

        void reload ()
        {
            std::filesystem::path dir = "../data";
            if (!std::filesystem::exists (dir / "vocabulary.dat")) return;
            std::ifstream ifstream (dir / "vocabulary.dat", std::ios::binary);

            dat::in::pool pool;
            pool.bytes.resize(3*4);
            ifstream.read((char*)(pool.bytes.data()), 3*4);

            dat::in::endianness = pool.get_int();
            int size_unzip = pool.get_int();
            int size_zip = pool.get_int();

            array<std::byte> zip;
            zip.resize(size_zip);
            ifstream.read((char*)(zip.data()), size_zip);
            pool.bytes = aux::unzip(zip.whole(), size_unzip).value();
            pool.offset = 0;
            pool.name = "vocabulary";

            eng::vocabulary.resize(pool.get_int());
            for (auto & word : eng::vocabulary)
                word = pool.get_string();
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

                splitter.lower = W * 5'000 / 10'000;
                splitter.upper = W * 9'000 / 10'000;

                int p = sys::settings::load("app::dic::splitter.permyriad", 10'000 * (W-w)/W);
                int x = aux::clamp<int>(W*p / 10'000, splitter.lower.now, splitter.upper.now);

                splitter.coord = XYXY(x-d, 0, x+d, H);

                card.coord = XYXY(0, 0, x, H);
                list.coord = XYXY(x, 0, W, H);
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

