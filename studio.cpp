#include "studio_app_dict.h"
#include "studio_app_dual.h"
#include "studio_app_mono.h"
#include "studio_build.h"
#include "studio_audio.h"
#include "studio_video.h"

struct Studio : gui::widget<Studio>
{
    gui::canvas canvas;
    gui::canvas toolbar;
    gui::radio::group select;
    array<gui::base::widget*> studios;
    studio::dict::studio dict;
    studio::dual::studio dual;
    studio::mono::studio mono;
    studio::audio::studio audio;
    studio::video::studio video;
    studio::build::studio build;

    Studio()
    {
        skin = "gray";
        gui::skins[skin].font =
        sys::font{"Segoe UI", gui::metrics::text::height};
        toolbar.color = gui::skins[skin].light.first;
        canvas .color = gui::skins[skin].light.first;

        select(0).text.text = "dictionary";
        select(1).text.text = "en-ru";
        select(2).text.text = "en";
        select(3).text.text = "audio";
        select(4).text.text = "video";
        select(5).text.text = "build!";
        select(0).on = true;

        studios += &dict;
        studios += &dual;
        studios += &mono;
        studios += &audio;
        studios += &video;
        studios += &build;

        for (int i=1; i<studios.size(); i++)
            studios[i]->hide();
    }

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*10;
            int h = gui::metrics::text::height*12/7;

            canvas .coord = XYWH(0, 0, W, H);
            toolbar.coord = XYWH(0, 0, W, h);

            select(0).coord = XYWH(w*0, 0, w, h);
            select(1).coord = XYWH(w*1, 0, w, h);
            select(2).coord = XYWH(w*2, 0, w, h);
            select(3).coord = XYWH(w*4, 0, w, h);
            select(4).coord = XYWH(w*5, 0, w, h);
            select(5).coord = XYWH(w*7, 0, w, h);
            select   .coord = XYWH(0, 0, 8*w, h);

            for (int i=0; i<studios.size(); i++)
                studios[i]->coord = XYXY(0, h, W, H);
        }
    }

    void on_notify (void* what) override
    {
        if (what == &select)
        {
            int n = select.notifier_index;

            for (int i=0; i<studios.size(); i++)
                studios[i]->show (i == n);

            if (studios[n] == &build)
                for (int i=0; i<studios.size(); i++)
                    if (i != n) select(i).enabled = false;

            if (studios[n] == &build)
                build.run();
        }

        if (what == &build)
        {
            if (build.data_updated)
            {
                dict.reload();
            }

            for (int i=0; i<studios.size(); i++)
                select(i).enabled = true;
        }
    }

    void on_focus (bool on) override
    {
        for (auto studio : studios) {
            if (studio->alpha.now == 255) {
                studio->on_focus(on);
                break;
            }
        }
    }
    void on_keyboard_input (str symbol) override
    {
        for (auto studio : studios) {
            if (studio->alpha.now == 255) {
                studio->on_keyboard_input(symbol);
                break;
            }
        }
    }
    void on_key_pressed (str key, bool down) override
    {
        for (auto studio : studios) {
            if (studio->alpha.now == 255) {
                studio->on_key_pressed(key,down);
                break;
            }
        }
    }
};

sys::app<Studio> application ("en studio");

#include "../ae/proto-studio/windows_fonts.h"
#include "../ae/proto-studio/windows_images.h"
#include "../ae/proto-studio/windows_system.h"
#include "../ae/proto-studio/windows_windows.h"
