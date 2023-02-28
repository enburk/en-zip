#include "studio_app_dic.h"
#include "studio_app_one.h"
#include "studio_app_two.h"
#include "studio_aux_audio.h"
#include "studio_aux_video.h"
#include "studio_build.h"

struct Studio : gui::widget<Studio>
{
    gui::toolbar canvas;
    gui::toolbar toolbar;
    gui::radio::group select;
    array<gui::base::widget*> studios;
    studio::dic::studio dic;
    studio::one::studio one;
    studio::two::studio two;
    studio::audio::studio audio;
    studio::video::studio video;
    studio::build::studio build;
    gui::button schema;
    gui::button app;

    Studio()
    {
        skin = "gray+";
        schema.text.text = "light";
        app.text.text = "app";

        studios += &one;
        studios += &two;
        studios += &dic;
        studios += &audio;
        studios += &video;
        studios += &build;

        for (int i=1; i<studios.size(); i++)
            studios[i]->hide();

        int i = 0;
        select(i++).text.text = "course";
        select(i++).text.text = "catalogs";
        select(i++).text.text = "dictionary";
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
            select(5).coord = xywh(w*7, 0, w, h);
            select(6).coord = xywh(w*8, 0, w, h);
            select   .coord = xywh(0, 0, 9*w, h);

            for (int i=0; i<studios.size(); i++)
                studios[i]->coord = xyxy(0, h, W, H);

            schema.coord = xywh(W-2*w, 0, w, h);
            app   .coord = xywh(W-1*w, 0, w, h);
        }

        if (what == &select)
        {
            int n = select.notifier_index;
            int s = n == 6 ? 5 : n;

            for (int i=0; i<studios.size(); i++)
                studios[i]->show(i == s);

            focus = studios[s];

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
                app::appdata.reload();
                dic.reload();
                one.reload();
                two.reload();
            }

            for (int i=0; i<studios.size(); i++)
                select(i).enabled = true;
        }

        if (what == &schema)
        {
            skin =
            schema.text.text == "light" ? "gray" : "gray+";
            schema.text.text =
            schema.text.text == "light" ? "dark" : "light";
        }

        if (what == &app)
        {
            auto path = ".vstudioapp/x64/Release/en-app.exe";
            sys::process run(path, "",
            sys::process::options{});
        }
    }
};

sys::app<Studio> application ("en studio");

#include "../auxs/windows_fonts.h"
#include "../auxs/windows_images.h"
#include "../auxs/windows_system.h"
#include "../auxs/windows_windows.h"
