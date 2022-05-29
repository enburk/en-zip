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
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*12/10;

                toolbar.coord = xywh(0, 0, W, h);

                select(0).coord = xywh(w*0, 0, w, h);
                select(1).coord = xywh(w*1, 0, w, h);
                select(2).coord = xywh(w*2, 0, w, h);
                select(3).coord = xywh(w*3, 0, w, h);
                select   .coord = xywh(0, 0, W, h);

                for (auto c: consoles)
                    c->coord = xyxy(0, h, W/2, H);

                detail.coord = xyxy(W/2, h, W, H);
            }

            if (what == &select)
            {
                int n = select.notifier_index;
                for (int i=0; i<consoles.size(); i++)
                    consoles[i]->show(i == n);
            }
        }
    };
}
