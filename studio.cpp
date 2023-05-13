#include "en-app.h"
#include "studia_dic.h"
#include "studia_one.h"
#include "studia_two.h"
#include "studio_pix.h"
#include "studio_aux_audio.h"
#include "studio_aux_video.h"
#include "studio_build.h"

struct Studio:
widget<Studio>
{
    gui::toolbar canvas;
    gui::toolbar toolbar;
    gui::radio::group select;
    array<gui::base::widget*> studios;
    studio::dic::studio dic;
    studio::one::studio one;
    studio::two::studio two;
    studio::pixer::studio pixer;
    studio::audio::studio audio;
    studio::video::studio video;
    studio::build::studio build;
    gui::button button_sch;
    gui::button button_app;
    gui::button button_App;
    App app;

    Studio()
    {
        skin = "gray+";
        button_sch.text.text = "light";
        button_app.text.text = "in-app";
        button_App.text.text = "app";

        app.hide();

        studios += &one;
        studios += &two;
        studios += &dic;
        studios += &pixer;
        studios += &audio;
        studios += &video;
        studios += &build;

        for (int i=1; i<
        studios.size(); i++)
        studios[i]->hide();

        int i = 0;
        select(i++).text.text = "course";
        select(i++).text.text = "catalogs";
        select(i++).text.text = "dictionary";
        select(i++).text.text = "snapshot";
        select(i++).text.text = "audio";
        select(i++).text.text = "video";
        select(i++).text.text = "logs";
        select(i++).text.text = "build!";
        select(0).on = true;
    }

    void on_change (void* what) override
    {
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*7;
            int h = gui::metrics::text::height*13/10;

            canvas .coord = xywh(0, 0, W, H);
            toolbar.coord = xywh(0, 0, W, h);

            select(0).coord = xywh(w*0, 0, w, h);
            select(1).coord = xywh(w*1, 0, w, h);
            select(2).coord = xywh(w*2, 0, w, h);
            select(3).coord = xywh(w*4, 0, w, h);
            select(4).coord = xywh(w*5, 0, w, h);
            select(5).coord = xywh(w*6, 0, w, h);
            select(6).coord = xywh(w*8, 0, w, h);
            select(7).coord = xywh(w*9, 0, w, h);
            select   .coord = xywh(0,0, 10*w, h);

            for (int i=0; i<studios.size(); i++)
            studios[i]->coord = xyxy(0, h, W, H);

            button_sch.coord = xywh(W-3*w, 0, w, h);
            button_App.coord = xywh(W-2*w, 0, w, h);
            button_app.coord = xywh(W-1*w, 0, w, h);

            app.coord = coord.now.local();
        }

        if (what == &select)
        {
            int n = select.notifier_index;
            int s = n == 7 ? 6 : n;

            for (int i=0; i<
            studios.size(); i++)
            studios[i]->show(i == s);

            focus = studios[s];

            if (n == 7) // build!
            for (int i=0; i<
            studios.size()-1; i++)
            select(i).enabled =
                false;

            if (n == 7)
            one.save(),
            build.run();
        }

        if (what == &build)
        {
            if (build.data_updated)
            {
                app::appdata.reload();
                dic.reload();
                one.reload();
                two.reload();
                app.reload();
            }

            for (int i=0; i<
            studios.size(); i++)
            select(i).enabled =
                true;
        }

        if (what == &button_sch)
        {
            skin =
            button_sch.text.text == "light" ? "gray" : "gray+";
            button_sch.text.text =
            button_sch.text.text == "light" ? "dark" : "light";
        }

        if (what == &button_App)
        {
            auto path = ".vstudioapp/x64/Release/en-app.exe";
            sys::process run(path, "",
            sys::process::options{});
        }

        if (what == &button_app)
        {
            app.show();
        }
    }

    void on_key(str key, bool down, bool input) override
    {
        if (down
        and key == "escape"
        and app.alpha.to != 0)
            app.hide();
        else
        if (focus.now)
            focus.now->on_key(
            key, down, input);
    }
};

sys::app<Studio> application ("en studio");

#include "../auxs/windows_fonts.h"
#include "../auxs/windows_images.h"
#include "../auxs/windows_system.h"
#include "../auxs/windows_windows.h"
