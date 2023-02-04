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
        int clicked = 0;

        eng::dictionary::entry current_entry;
        eng::dictionary::index current_index;

        area ()
        {
            card.object.text.scroll.x.mode = gui::scroll::mode::none;
            card.object.text.alignment = xy{pix::left, pix::top};
            card.object.text.mouse_wheel_speed = 2.0;
            undo.text.ellipsis = true;
            redo.text.ellipsis = true;
            refresh();
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

            timing t0;
            std::filesystem::path dir = "../data";
            if (!std::filesystem::exists(dir/"dictionary_indices.dat")) return;
            if (!std::filesystem::exists(dir/"dictionary_entries.dat")) return;
            std::ifstream indices_stream(dir/"dictionary_indices.dat", std::ios::binary);
            std::ifstream entries_stream(dir/"dictionary_entries.dat", std::ios::binary);
            dat::in::pool indices_pool;
            dat::in::pool entries_pool;

            indices_stream.seekg(n*eng::dictionary::index::size, std::ios::beg);
            indices_pool.bytes.resize(eng::dictionary::index::size);
            indices_stream.read((char*)(indices_pool.bytes.data()),
            eng::dictionary::index::size);
            eng::dictionary::index index;
            index << indices_pool;

            if (index.redirect >= 0) n =
                index.redirect;
            if (index.length == 0) return;
            if (index.length == current_index.length
            and index.offset == current_index.offset)
                return;

            entries_stream.seekg(index.offset, std::ios::beg);
            entries_pool.bytes.resize(index.length);
            entries_stream.read((char*)(entries_pool.bytes.data()), index.length);
            eng::dictionary::entry entry;
            entry << entries_pool;

            if (undoes.size() > 1000)
                undoes.erase(
                undoes.begin(),
                undoes.begin()
                +500);

            if (current_entry.title != "")
            undoes += current_entry.title;
            redoes.clear();

            current_entry = entry;
            current_index = index;

            array<str> links;
            links += entry.title;
            for (auto redirect : entry.redirects)
            links += vocabulary[redirect].title;

            timing t1;
            str debug = "";///"<br> n = " + std::to_string(n);
            str html = wiki2html(entry, links) + debug;
            if (false) std::ofstream("test.html") << html;
            if (false) std::ofstream("test.html.txt") << doc::html::print(html);
            if (false)
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

            timing t2;
            card.object.text.html = html;
            card.object.text.scroll.y.top = 0;
            refresh();

            timing t3;
            str title = doc::html::encoded(entry.title);
            logs::times << "<br>" + title;
            logs::media << "<br>" + title;
            logs::audio << "<br>" + title;
            logs::video << "<br>" + title;
            auto selected = media::select(n);

            timing t4; card.object.reset(title, selected.video, selected.vudio, links);
            timing t5; quot.object.reset(title, selected.audio, links);
            timing t6;

            logs::times << gray(monospace(
            "time file  " + format(t1-t0) + " sec<br>" +
            "time wiki  " + format(t2-t1) + " sec<br>" +
            "time html  " + format(t3-t2) + " sec<br>" +
            "time media " + format(t4-t3) + " sec<br>" +
            "time video " + format(t5-t4) + " sec<br>" +
            "time audio " + format(t6-t5) + " sec<br>" +
            "time total " + format(t6-t0) + " sec"));
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
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width;
                int w = W/2;

                int htool = h*12/10;
                int hquot = 6*htool + 3*htool/5 + 0*l;
                int hcard = H - htool - hquot;
                int y = 0;

                card.coord = xywh(0, 0, W, hcard); y += card.coord.now.h;
                quot.coord = xywh(0, y, W, hquot); y += quot.coord.now.h;
                tool.coord = xyxy(0, y, W, H);
                undo.coord = xyxy(0, y, w, H);
                redo.coord = xyxy(w, y, W, H);
            }
            if (what == &skin)
            {
                tool.color = gui::skins[skin].light.first;
            }

            if (what == &card.object) { clicked = card.object.clicked; notify(); }
            if (what == &quot.object) { clicked = quot.object.clicked; notify(); }

            if (what == &undo and not undoes.empty()) 
            {
                str link =
                undoes.back();
                undoes.pop_back();
                redoes += current_entry.title;
                auto saving = std::move(redoes);
                auto index = vocabulary.index(link);
                if (not index) { str s = "undoes: not found: " + link;
                logs::times << s;
                logs::media << s;
                logs::audio << s;
                logs::video << s;
                return; }
                clicked = *index;
                notify();
                undoes.pop_back();
                redoes = std::move(saving);
                refresh();
            }
            if (what == &redo and not redoes.empty()) 
            {
                str link =
                redoes.back();
                redoes.pop_back();
                auto saving = std::move(redoes);
                auto index = vocabulary.index(link);
                if (not index) { str s = "redoes: not found: " + link;
                logs::times << s;
                logs::media << s;
                logs::audio << s;
                logs::video << s;
                return; }
                clicked = *index;
                notify();
                redoes = std::move(saving);
                refresh();
            }
            if (what == &quot.object.mute)
            {
                card.object.mute =
                quot.object.mute.on.now;
            }
        }
    };
}

