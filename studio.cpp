#include "studio_app_dict.h"
#include "studio_app_dual.h"
#include "studio_app_mono.h"
#include "studio_compile.h"
#include "studio_audio.h"
#include "studio_video.h"

struct Studio : gui::widget<Studio>
{
    gui::canvas toolbar;
    gui::radio::group select;
    studio::dict::studio dict;
    studio::dual::studio mono;
    studio::mono::studio dual;
    studio::audio::studio audio;
    studio::video::studio video;
    studio::compile::studio compile;
    array<gui::base::widget*> studios;

    Studio()
    {
        gui::window = this;

        skin = "gray";

        select(0).text.text = "dictionary";
        select(1).text.text = "bilingual";
        select(2).text.text = "monolingual";
        select(3).text.text = "audio";
        select(4).text.text = "video";
        select(5).text.text = "compile!";
        select(0).on = true;

        studios += &dict;
        studios += &mono;
        studios += &dual;
        studios += &audio;
        studios += &video;
        studios += &compile;

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
            int h = gui::metrics::text::height*2;

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
        if (what == &skin)
        {
            toolbar.color = gui::skins[skin.now].light.back_color;
        }
    }

    void on_notify (gui::base::widget* w, int n) override
    {
        if (w == &select)
        {
            for (int i=0; i<studios.size(); i++)
                studios[i]->show (i == n);

            if (studios[n] == &compile)
                for (int i=0; i<studios.size(); i++)
                    if (i != n) select(i).enabled = false;

            compile.run();
        }
    }
    void on_notify (gui::base::widget* w) override
    {
        if (w == &compile)
        {
            if (compile.data_updated)
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

sys::app<Studio> application ("EN studio");

#include "../ae/library/cpp/platforms/microsoft_windows_fonts.h"
#include "../ae/library/cpp/platforms/microsoft_windows_images.h"
#include "../ae/library/cpp/platforms/microsoft_windows_windows.h"
