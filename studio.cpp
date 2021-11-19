#include "studio_app_dic.h"
#include "studio_app_one.h"
#include "studio_app_two.h"
#include "studio_aux_audio.h"
#include "studio_aux_video.h"
#include "studio_build.h"

struct Studio : gui::widget<Studio>
{
    gui::canvas canvas;
    gui::canvas toolbar;
    gui::radio::group select;
    array<gui::base::widget*> studios;
    studio::dic::studio dic;
    studio::one::studio one;
    studio::two::studio two;
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

        studios += &dic;
        studios += &one;
        studios += &two;
        studios += &audio;
        studios += &video;
        studios += &build;

        for (int i=1; i<studios.size(); i++)
            studios[i]->hide();

        select(0).text.text = "dictionary";
        select(1).text.text = "en";
        select(2).text.text = "en-ru";
        select(3).text.text = "audio";
        select(4).text.text = "video";
        select(5).text.text = "build";
        select(6).text.text = "build!";
        select(0).on = true;
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
            select(6).coord = XYWH(w*8, 0, w, h);
            select   .coord = XYWH(0, 0, 9*w, h);

            for (int i=0; i<studios.size(); i++)
                studios[i]->coord = XYXY(0, h, W, H);
        }
    }

    void on_notify (void* what) override
    {
        if (what == &select)
        {
            int n = select.notifier_index;
            int s = n == 6 ? 5 : n;

            for (int i=0; i<studios.size(); i++)
                studios[i]->show (i == s);

            if (n == 6) // build!
                for (int i=0; i<studios.size()-1; i++)
                    select(i).enabled = false;

            if (n == 6)
                build.run();
        }

        if (what == &build)
        {
            if (build.data_updated)
            {
                dic.app.reload(); dic.reload();
                one.app.reload(); one.reload();
                two.app.reload(); two.reload();
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
