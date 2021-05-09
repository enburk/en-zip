#pragma once
#include "app_dict_card_card.h"
#include "app_dict_card_quot.h"
namespace app::dict::card
{
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

        void reload ()
        {
            card.object.reload();
            quot.object.reload();
        }

        void select (int n)
        {
            /// if (log) *log <<
            /// "app::dict::card::select "
            /// + std::to_string(n) + " "
            /// + vocabulary[n].title + " -> "
            /// + std::to_string(vocabulary[n].redirect);

            if (n < 0) return;
            if (n >= vocabulary.size()) return;
            if (vocabulary[n].redirect >= 0) n =
                vocabulary[n].redirect;
            if (vocabulary[n].length == 0) return;
            if (vocabulary[n].length == current.length
            and vocabulary[n].offset == current.offset) return;

            /// if (log) *log <<
            /// "app::dict::card::select redirected "
            /// + std::to_string(n) + " "
            /// + vocabulary[n].title;

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

            array<str> excluded_links;
            excluded_links += entry.title;
            for (auto redirect : entry.redirects)
                excluded_links += vocabulary[redirect].title;

            card.object.text.excluded_links = 
            quot.object.excluded_links = 
                excluded_links;

            str debug = "";///"<br> n = " + std::to_string(n);
            str html = wiki2html(entry) + debug;
            if (true) std::ofstream("test.html") << html;
            if (true) std::ofstream("test.html.txt") << doc::html::print(html);
            card.object.text.html = html;
            card.object.text.scroll.y.top = 0;
            refresh();

            mediae::select(n);
            card.object.reset_image();
            quot.object.reset_audio();
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
                int hquot = 4*h*12/7 + 6*l;
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

