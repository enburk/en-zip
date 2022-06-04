#pragma once
#include "studio_app_dic_detail.h"
namespace studio::dic
{
    struct area:
    widget<area>
    {
        gui::area<gui::canvas> toolbar;
        gui::area<gui::canvas> consbar;
        gui::area<detail> details;

        gui::radio::group select;
        gui::console log_times;
        gui::console log_media;
        gui::console log_audio;
        gui::console log_video;
        gui::splitter splitter;

        array<gui::console*> consoles;

        area ()
        {
            toolbar.object.color = gui::skins[skin].light.first;

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

            log_media.view.wordwrap = false;
            log_audio.view.wordwrap = false;
            log_video.view.wordwrap = false;
            log_media.view.ellipsis = true;
            log_audio.view.ellipsis = true;
            log_video.view.ellipsis = true;
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*13/10;
                int d = gui::metrics::line::width*10;

                splitter.lower = W * 25'00 / 100'00;
                splitter.upper = W * 75'00 / 100'00;
                str s = "studio::dic::area::splitter.permyriad";
                int p = sys::settings::load(s, 40'00);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower, splitter.upper);
                splitter.coord = xyxy(x-d, 0, x+d, H);

                toolbar.coord = xywh(0, 0, W, h);
                consbar.coord = xyxy(0, h, x, H);
                details.coord = xyxy(x, h, W, H);

                select.coord = toolbar.object.coord.now;
                select(0).coord = xywh(w*0, 0, w, h);
                select(1).coord = xywh(w*1, 0, w, h);
                select(2).coord = xywh(w*2, 0, w, h);
                select(3).coord = xywh(w*3, 0, w, h);

                for (auto c: consoles) c->coord =
                consbar.object.coord.now +
                consbar.coord.now.origin;
            }

            if (what == &splitter) {
                sys::settings::save(
                "studio::dic::area::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                on_change(&coord);
            }

            if (what == &select)
            {
                int n = select.notifier_index;
                for (int i=0; i<consoles.size(); i++)
                    consoles[i]->show(i == n);
            }

            if (what == &details)
                notify();

            if (what ==  &log_media.link)
                details.object.select(
                    log_media.link);
            else
            if (what ==  &log_audio.link)
                details.object.select(
                    log_audio.link);
            else
            if (what ==  &log_video.link)
                details.object.select(
                    log_video.link);
        }
    };
}
