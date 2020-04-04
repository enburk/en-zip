#include "app_dict_html.h"
namespace app::dict::card
{
    auto font = [](){ return sys::font{"Segoe UI", gui::metrics::text::height}; };

    struct card : gui::widget<card>
    {
        html_view text;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                text.coord = coord.now.local();
            }
            if (what == &skin)
            {
                text.view.canvas.color = gui::skins[skin.now].white;
                text.font = font();
            }
        }

        void on_notify (gui::base::widget* w, int n) override
        {
            if (w == &text) notify(n);
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
        gui::button undo;
        gui::button redo;
        array<str> undoes;
        array<str> redoes;

        eng::vocabulary::entry current;

        area ()
        {
            card.object.text.alignment = XY{gui::text::left, gui::text::top};
            card.object.text.mouse_wheel_speed = 2.0;
            undo.text.text = "undo";
            redo.text.text = "redo";
        }

        void select (int n)
        {
            if (n >= vocabulary.size()) return;
            if (vocabulary[n].length == 0) return;
            if (vocabulary[n].length == current.length
            &&  vocabulary[n].offset == current.offset) return;

            if (current.title != "")
            undoes += current.title;
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

            str html = wiki2html(entry);
            if (true) std::ofstream("test.html") << html;
            card.object.text.html = html;
            card.object.text.scroll.y.top = 0;
            refresh();
        }

        void refresh ()
        {
            undo.text.text = undoes.size() > 0 ? undoes.back() : "undo";
            redo.text.text = redoes.size() > 0 ? redoes.back() : "redo";
            undo.enabled   = undoes.size() > 0;
            redo.enabled   = redoes.size() > 0;
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

                card.coord = XYWH(0, 0, W, n*h + 2*l); y += card.coord.now.h; h -= l*2/3;
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

        void on_notify (gui::base::widget* w, int n) override
        {
            if (w == &card) notify(n);
            if (w == &quot) notify(n);
        }
        void on_notify (gui::base::widget* w) override
        {
            if (w == &undo && undoes.size() > 0) 
            {
                str link = undoes.back();
                undoes.pop_back(); redoes += current.title;
                if (auto range = vocabulary_range(link); range) {
                    notify(range.offset);
                    undoes.pop_back();
                    refresh();
                }
            }
            if (w == &redo && redoes.size() > 0) 
            {
                str link = redoes.back(); redoes.pop_back();
                if (auto range = vocabulary_range(link); range)
                    notify(range.offset);
            }
        }
    };
}

