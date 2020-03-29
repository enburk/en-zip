#include "studio_dic.h"
#include "studio_pix.h"
#include "studio_rus.h"

struct Studio : gui::widget<Studio>
{
    gui::canvas toolbar;
    gui::button button_dic;
    gui::button button_pix;
    gui::button button_rus;
    gui::button button_img;

    studio::dic::studio s;


    Studio()
    {
        gui::window = this;
        skin = "gray";
        //image<RGBA> img = pix::read("test.jpg").value();
        //pix::write(img, "test.png");
    }

    void on_change (void* what) override
    {
        if (what == &coord && coord.was.size != coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height;
            int l = gui::metrics::line::width*3;
            int w = W/2;

            s.coord = XYWH(0, 0, W, H);
        }
    }

    void on_notify (gui::base::widget* w) override
    {
        if (w == &button_dic)
        {
        }
    }

    void on_focus (bool on) override { s.on_focus(on); }
    void on_keyboard_input (str symbol) override { s.on_keyboard_input(symbol); }
    void on_key_pressed (str key, bool down) override { s.on_key_pressed(key,down); }
};
sys::app<Studio> application ("EN studio");

#include "../ae/library/cpp/platforms/microsoft_windows_fonts.h"
#include "../ae/library/cpp/platforms/microsoft_windows_images.h"
#include "../ae/library/cpp/platforms/microsoft_windows_windows.h"
