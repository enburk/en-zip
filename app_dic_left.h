#pragma once
#include "app_dic_html_wiki.h"
#include "app_dic_left_card.h"
#include "app_dic_left_quot.h"
namespace app::dic::left
{
    struct area:
    widget<area>
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
            if (n < 0) return;
            if (n >= vocabulary.data.size()) return;
            if (vocabulary[n].redirect >= 0) n =
                vocabulary[n].redirect;
            if (vocabulary[n].length == 0) return;
            if (vocabulary[n].length == current.length
            and vocabulary[n].offset == current.offset) return;

            sys::timing t0;
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
            entry << pool;

            array<str> links;
            links += entry.title;
            for (auto redirect : entry.redirects)
            links += vocabulary[redirect].title;

            sys::timing t1;
            str debug = "";///"<br> n = " + std::to_string(n);
            str html = wiki2html(entry, links) + debug;
            if (true) std::ofstream("test.html") << html;
            if (true) std::ofstream("test.html.txt") << doc::html::print(html);
            if (true)
            {
                array<str> ss;
                ss += entry.title; ss += "";
                for (auto topic: entry.topics) {
                    ss += "  " + topic.header + " == " + topic.forms;
                    for (auto s: topic.content) ss += "    " + s;
                    ss += "";
                }
                std::ofstream("test.txt") << str(ss);
            }

            sys::timing t2;
            card.object.text.html = html;
            card.object.text.scroll.y.top = 0;
            refresh();

            sys::timing t3; auto selected = media::select(n);
            sys::timing t4; card.object.reset(selected.video, links);
            sys::timing t5; quot.object.reset(selected.audio, links);
            sys::timing t6;

            log << doc::html::encoded(entry.title) + "<br>" 
            "<font color=#808080 face=\"monospace\">" +
            "time file  " + sys::format(t1-t0) + " sec<br>"+
            "time wiki  " + sys::format(t2-t1) + " sec<br>"+
            "time html  " + sys::format(t3-t2) + " sec<br>"+
            "time media " + sys::format(t4-t3) + " sec<br>"+
            "time video " + sys::format(t5-t4) + " sec<br>"+
            "time audio " + sys::format(t6-t5) + " sec<br>"+
            "time total " + sys::format(t6-t0) + " sec<br>"+
            "</font>";
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
                int hquot = 6*h*12/7 + 3*h*12/7/5 + 0*l;
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
                undoes.pop_back();
                redoes += current.title;
                auto index = vocabulary.index(link);
                if (not index) { log << "undoes: not found: " + link; return; }
                clicked = *index;
                notify();
                undoes.pop_back();
                refresh();
            }
            if (what == &redo && redoes.size() > 0) 
            {
                str link = redoes.back();
                redoes.pop_back();
                auto index = vocabulary.index(link);
                if (not index) { log << "redoes: not found: " + link; return; }
                clicked = *index;
                notify();
            }
        }
    };
}

