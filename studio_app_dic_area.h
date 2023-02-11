#pragma once
#include "studio_app_dic_detail.h"
#include "studio_app_dic_search.h"
namespace studio::dic
{
    struct area:
    widget<area>
    {
        gui::area<gui::canvas> toolbar;
        gui::area<gui::canvas> consbar;
        gui::area<detail> detail;
        gui::area<search> search;

        gui::radio::group select;
        gui::console log_media;
        gui::console log_audio;
        gui::console log_video;
        gui::console log_search;
        gui::console log_timing;
        gui::splitter splitter;

        array<gui::console*> consoles;

        int clicked = 0;

        area ()
        {
            consoles += &log_media;
            consoles += &log_audio;
            consoles += &log_video;
            consoles += &log_search;
            consoles += &log_timing;

            for (int i=1; i<consoles.size(); i++)
                consoles[i]->hide();

            search.object.result = &log_search;

            app::logs::times = log_timing;
            app::logs::media = log_media;
            app::logs::audio = log_audio;
            app::logs::video = log_video;

            int i = 0;
            select(i++).text.text = "media";
            select(i++).text.text = "audio";
            select(i++).text.text = "video";
            select(i++).text.text = "search";
            select(i++).text.text = "timing";
            select(0).on = true;

            log_media .view.wordwrap = false;
            log_audio .view.wordwrap = false;
            log_video .view.wordwrap = false;
            log_search.view.wordwrap = false;
            log_media .view.ellipsis = true;
            log_audio .view.ellipsis = true;
            log_video .view.ellipsis = true;
            log_search.view.ellipsis = true;
            log_search.view.current_line_frame.color =
                rgba(150,150,150,64);

            if (app::appdata.error != "")
                log_media << red(bold(
                app::appdata.error));
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
                int l = gui::metrics::line::width;

                splitter.lower = W * 25'00 / 100'00;
                splitter.upper = W * 75'00 / 100'00;
                str s = "studio::dic::area::splitter.permyriad";
                int p = sys::settings::load(s, 60'00);
                int x = clamp<int>(W*p / 100'00,
                splitter.lower, splitter.upper);
                splitter.coord = xyxy(x-10*l, 0, x+10*l, H);

                toolbar.coord = xywh(0, 0, W, h);
                consbar.coord = xyxy(x, h, W, H);
                detail .coord = xyxy(0, h, x, H);
                search .coord = xyxy(0, h, x, H);

                select.coord = toolbar.object.coord.now;
                select(0).coord = xywh(w*0, 0, w, h-6*l);
                select(1).coord = xywh(w*1, 0, w, h-6*l);
                select(2).coord = xywh(w*2, 0, w, h-6*l);
                select(3).coord = xywh(w*3, 0, w, h-6*l);
                select(4).coord = xywh(w*4, 0, w, h-6*l);

                for (auto c: consoles) c->coord =
                consbar.object.coord.now +
                consbar.coord.now.origin;
            }

            if (what == &skin)
            {
                toolbar.object.color = gui::skins[skin].light.first;
            }

            if (what == &splitter) {
                sys::settings::save(
                "studio::dic::area::splitter.permyriad",
                splitter.middle * 100'00 / coord.now.w);
                coord.was.size = xy{};
                on_change(&coord);
            }

            if (what == &select)
            {
                int n = select.notifier_index;
                for (int i=0; i<consoles.size(); i++)
                    consoles[i]->show(i == n);

                detail.show(n == 0 or n == 1 or n == 2);
                search.show(n == 3);
            }

            if (what == &detail) {
                clicked = detail.object.clicked;
                notify();
            }
            else
            if (what ==  &log_media.link)
                detail.object.select(
                    log_media.link);
            else
            if (what ==  &log_audio.link)
                detail.object.select(
                    log_audio.link);
            else
            if (what ==  &log_video.link)
                detail.object.select(
                    log_video.link);
            else
            if (what ==  &log_search.link)
            {
                if (auto index =
                app::vocabulary.index
                (log_search.link); index)
                {
                    clicked = *index;
                    notify();
                }
            }
        }
    };
}
