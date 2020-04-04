#include "studio_dict.h"
#include "studio_dual.h"
#include "studio_mono.h"

struct Studio : gui::widget<Studio>
{
    gui::canvas toolbar;
    gui::radio::group select;
    studio::dict::studio dict;
    studio::dual::studio mono;
    studio::mono::studio dual;

    Studio()
    {
        gui::window = this;

        skin = "gray";

        select(0).text.text = "dictionary";
        select(1).text.text = "bilingual";
        select(2).text.text = "monolingual";
        select(0).on = true;
        mono.hide();
        dual.hide();

        //image<RGBA> img = pix::read("test.jpg").value();
        //pix::write(img, "test.png");
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
            select   .coord = XYWH(0, 0, 3*w, h);

            dict.coord = XYXY(0, h, W, H);
            dual.coord = XYXY(0, h, W, H);
            mono.coord = XYXY(0, h, W, H);
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
            dict.show (n == 0);
            dual.show (n == 1);
            mono.show (n == 2);
        }
    }

    void on_focus (bool on) override
    {
        if (dict.alpha.now == 255) dict.on_focus(on); else
        if (dual.alpha.now == 255) dual.on_focus(on); else
        if (mono.alpha.now == 255) mono.on_focus(on);
    }
    void on_keyboard_input (str symbol) override
    {
        if (dict.alpha.now == 255) dict.on_keyboard_input(symbol); else
        if (dual.alpha.now == 255) dual.on_keyboard_input(symbol); else
        if (mono.alpha.now == 255) mono.on_keyboard_input(symbol);
    }
    void on_key_pressed (str key, bool down) override
    {
        if (dict.alpha.now == 255) dict.on_key_pressed(key,down); else
        if (dual.alpha.now == 255) dual.on_key_pressed(key,down); else
        if (mono.alpha.now == 255) mono.on_key_pressed(key,down);
    }
};

sys::app<Studio> application ("EN studio");

#include "../ae/library/cpp/platforms/microsoft_windows_fonts.h"
#include "../ae/library/cpp/platforms/microsoft_windows_images.h"
#include "../ae/library/cpp/platforms/microsoft_windows_windows.h"
