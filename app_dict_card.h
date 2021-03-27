#pragma once
#include "app_dict_media.h"
#include "app_dict_video.h"
namespace app::dict::card
{
    struct card : gui::widget<card>
    {
        html_view text;
        video::sequencer image;

        void reset_image ()
        {
            image.reset();

            int l = gui::metrics::line::width;

            XY size;
            for (auto video : mediae::selected_video)
            size.x = max (size.x, video.location.size_x + 6*l);
            size.y = image.height(size.x);

            refresh_image(size);
        }

        void refresh_image (XY size)
        {
            int maxwidth = coord.now.size.x * 2/3;
            if (maxwidth < size.x) size = XY (
                maxwidth, image.height(maxwidth));

            text.margin_right = size;
            int d = text.scroll.y.alpha.to == 0 ?
                0 : text.scroll.y.coord.now.w;

            image.coord = XYWH(
                coord.now.size.x - size.x - d, 0,
                size.x, size.y
            );
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                text.coord = coord.now.local();
                refresh_image(image.coord.now.size);
            }
            if (what == &skin)
            {
                text.view.canvas.color = gui::skins[skin].ultralight.first;
            }
            if (what == &image)
            {
                text.margin_right = image.coord.now.size;
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &text ) { clicked = text .clicked; notify(); }
            if (what == &image) { clicked = image.clicked; notify(); }
        }
    };

    struct quot : gui::widget<quot>
    {
        int clicked = 0;
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
            card.object.text.scroll.x.mode = gui::scroll::mode::none;
            card.object.text.alignment = XY{pix::left, pix::top};
            card.object.text.mouse_wheel_speed = 2.0;
            undo.text.text = "undo";
            redo.text.text = "redo";
        }

        void select (int n)
        {
            if (true) if (log) *log <<
            "app::dict::card::select "
            + std::to_string(n);

            if (n < 0) return;
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

            str debug = "<br> n = " + std::to_string(n);
            str html = wiki2html(entry) + debug;
            if (true) std::ofstream("test.html") << html;
            card.object.text.html = html;
            card.object.text.scroll.y.top = 0;
            refresh();

            mediae::select(n);
            card.object.reset_image();
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
                int l = gui::metrics::line::width;
                int w = W/2;

                int htool = h*12/7;
                int hquot = 5*h + 6*l;
                int hcard = H - htool - hquot;
                int y = 0;

                card.coord = XYWH(0, 0, W, hcard); y += card.coord.now.h;
                quot.coord = XYWH(0, y, W, hquot); y += quot.coord.now.h;
                tool.coord = XYXY(0, y, W, H);
                undo.coord = XYXY(0, y, w, H);
                redo.coord = XYXY(w, y, W, H);
            }
            if (what == &skin)
            {
                tool.color = gui::skins[skin].light.first;
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &card.object) { clicked = card.object.clicked; notify(); }
            if (what == &quot.object) { clicked = quot.object.clicked; notify(); }

            if (what == &undo && undoes.size() > 0) 
            {
                str link = undoes.back();
                undoes.pop_back(); redoes += current.title;
                if (const auto range = eng::vocabulary::
                    find_case_insensitive(link);
                    not range.empty()) {
                    clicked = range.offset();
                    notify();
                    undoes.pop_back();
                    refresh();
                }
            }
            if (what == &redo && redoes.size() > 0) 
            {
                str link = redoes.back(); redoes.pop_back();
                if (const auto range = eng::vocabulary::
                    find_case_insensitive(link);
                    not range.empty()) {
                    clicked = range.offset();
                    notify();
                }
            }
        }
    };
}

