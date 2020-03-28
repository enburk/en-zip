#include "app_dic_html.h"
namespace app::dic::card
{
    auto font = [](){ return sys::font{"Segoe UI", gui::metrics::text::height}; };

    struct card : gui::widget<card>
    {
        gui::text::view text;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                text.coord = coord.now.local();
            }
            if (what == &skin)
            {
                text.canvas.color = gui::skins[skin.now].white;
                text.font = font();
            }
        }
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

        eng::vocabulary::entry current;

        area ()
        {
            card.object.text.alignment = XY{gui::text::left, gui::text::top};
            undo.text.text = "undo";
            redo.text.text = "redo";
        }

        void select (int n)
        {
            if (n >= vocabulary.size()) return;
            if (vocabulary[n].length == 0) { select(eng::dictionary::entry{}); return; }
            if (vocabulary[n].length == current.length
            &&  vocabulary[n].offset == current.offset) return;
            current = vocabulary[n];

            std::filesystem::path dir = "../data";
            if (!std::filesystem::exists (dir / "dictionary.dat")) return;
            std::ifstream ifstream (dir / "dictionary.dat", std::ios::binary);
            ifstream.seekg(current.offset, std::ios::beg);

            dat::in::pool pool;
            pool.bytes.resize(current.length);
            ifstream.read((char*)(pool.bytes.data()), current.length);
            eng::dictionary::entry entry;
            pool >> entry;
            select(entry);
        }

        void select (eng::dictionary::entry entry)
        {
            str html = wiki2html(entry);

            if (true) std::ofstream("test.html") << html;

            card.object.text.html = html;
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

