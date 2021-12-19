#pragma once
#include "studio_app_dic_detail.h"
namespace studio::dic
{
    struct area:
    widget<area>
    {
        gui::canvas toolbar;
        gui::radio::group select;
        array<gui::console*> consoles;
        gui::console log_times;
        gui::console log_media;
        gui::console log_audio;
        gui::console log_video;
        detail detail;

        area ()
        {
            toolbar.color = gui::skins[skin].light.first;

            consoles += &log_times;
            consoles += &log_media;
            consoles += &log_audio;
            consoles += &log_video;

            for (int i=1; i<consoles.size(); i++)
                consoles[i]->hide();

            app::dic::logs::times = log_times;
            app::dic::logs::media = log_media;
            app::dic::logs::audio = log_audio;
            app::dic::logs::video = log_video;

            select(0).text.text = "times";
            select(1).text.text = "media";
            select(2).text.text = "audio";
            select(3).text.text = "video";
            select(0).on = true;
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*7;
                int h = gui::metrics::text::height*12/7;

                toolbar.coord = XYWH(0, 0, W, h);

                select(0).coord = XYWH(w*0, 0, w, h);
                select(1).coord = XYWH(w*1, 0, w, h);
                select(2).coord = XYWH(w*2, 0, w, h);
                select(3).coord = XYWH(w*3, 0, w, h);
                select   .coord = XYWH(0, 0, W, h);

                for (auto c: consoles)
                    c->coord = XYXY(0, h, W/2, H);

                detail.coord = XYXY(W/2, h, W, H);
            }

            if (what == &select)
            {
                int n = select.notifier_index;
                for (int i=0; i<consoles.size(); i++)
                    consoles[i]->show(i == n);
            }
        }

        void on_focus (bool on) override
        {
            detail.on_focus(on);
            if (not on)
            for (auto c: consoles)
                c->page.view.selections =
                array<gui::text::range>();
        }
        void on_keyboard_input (str symbol) override
        {
            for (auto c: consoles)
                c->page.view.selections =
                array<gui::text::range>();

            detail.on_keyboard_input(symbol);
        }
        void on_key_pressed (str key, bool down) override
        {
            if (key == "") return;
            if (key == "ctrl+C" or
                key == "ctrl+insert" or
                key == "shift+left"  or
                key == "shift+right" or
                key == "ctrl+left"   or
                key == "ctrl+right"  or
                key == "ctrl+shift+left"  or
                key == "ctrl+shift+right" or
                key == "shift+up"    or
                key == "shift+down")
            {
                for (auto c: consoles)
                    if (c->alpha.now > 0 and
                        c->page.view.selected() != "") {
                        c->page.on_key_pressed(key,down);
                        return; }
            }

            for (auto c: consoles)
                c->page.view.selections =
                array<gui::text::range>();

            detail.on_key_pressed(key,down);
        }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button != 'L' or not down) return;

            for (int i=0; i<consoles.size(); i++)
            {
                if (not select(i).on.now) continue;

                auto & console = *consoles[i];
                auto & column = console.page.view.column;
                p -= console.coord.now.origin;

                if (!column.coord.now.includes(p)) return;
                XY cp = p - column.coord.now.origin;
                for (auto & line : column)
                {
                    if (!line.coord.now.includes(cp)) continue;
                    XY lp = cp - line.coord.now.origin;
                    for (auto & token : line)
                    {
                        if (!token.coord.now.includes(lp)) continue;
                        auto rc = detail.select(token.info);
                        if (!rc.ok()) console << rc.error();
                        break;
                    }
                    break;
                }
                break;
            }
        }
    };
}
