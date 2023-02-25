#include "app_dic.h"
#include "app_one.h"
#include "app_two.h"
#include "app_contents.h"
#include "app_contents+.h"
#include "../auxs/sfx_playback.h"

struct App:
widget<App>
{
    gui::canvas canvas;
    app::one::app one;
    app::two::app two;
    app::dic::app dic;
    app::contents ones;
    app::contents twos;
    app::Contents Ones;
    app::Contents Twos;
    gui::splitter splitter1;
    gui::splitter splitter2;
    gui::selector onetwo;
    gui::button dicon;
    gui::button conon;

    sfx::playback play;
    gui::button slower;
    gui::button faster;
    gui::button mute;

    gui::console report;
    gui::console errors;

    App ()
    {
        skin = "gray+";
        canvas.color =
        gui::skins[skin].ultralight.first;
        dicon.kind = gui::button::toggle;
        conon.kind = gui::button::toggle;
        conon.text.text = "\xE2""\x98""\xB7";
        dicon.text.text = "dictionary";
        onetwo.buttons(0).text.text = "course";
        onetwo.buttons(1).text.text = "catalogs";
        onetwo.maxwidth = max<int>();
        onetwo.selected = 0;
        reload();
    }

    void reload () try
    {
        app::appdata.reload();
        ones.reload("app::ones", app::one::course.root);
        twos.reload("app::twos", app::two::course.root);
        Ones.reload("app::Ones", app::one::course.root);
        Twos.reload("app::Twos", app::two::course.root);
        dic.reload();
        one.reload();
        two.reload();
    }
    catch (std::exception const& e)
    {
        app::logs::errors <<
        bold(red(e.what()));
        errors.show();
    }

    void refresh ()
    {
        int W = coord.now.w; if (W <= 0) return;
        int H = coord.now.h; if (H <= 0) return;
        int w = gui::metrics::text::height*5;
        int v = gui::metrics::text::height*2;
        int h = gui::metrics::text::height*13/10;
        int d = gui::metrics::line::width*2;
        int l = splitter1.set("app::splitter1",  7,  9, 11);
        int r = splitter2.set("app::splitter2", 50, 60, 70);

        play  .coord = xywh(l+0*w+0*v, H-h+d, 2*w, h-d-d);

        conon .coord = xywh(r-3*w-1*v, H-h+d, 1*v, h-d-d);
        onetwo.coord = xywh(r-3*w-0*v, H-h+d, 2*w, h-d-d);
        dicon .coord = xywh(r-1*w-0*v, H-h+d, 1*w, h-d-d);

        if (not conon.on.now) l = 0;
        if (not dicon.on.now) r = W;

        one .show(onetwo.selected.now == 0);
        two .show(onetwo.selected.now == 1);
        ones.show(onetwo.selected.now == 0 and conon.on.now);
        twos.show(onetwo.selected.now == 1 and conon.on.now);
        dic .show(dicon.on.now);

        splitter1.show(conon.on.now);
        splitter2.show(dicon.on.now);

        canvas.coord = xyxy(0, 0, W, H);
        ones  .coord = xyxy(0, 0, l, H);
        twos  .coord = xyxy(0, 0, l, H);
        one   .coord = xyxy(l, 0, r, H-h);
        two   .coord = xyxy(l, 0, r, H-h);
        dic   .coord = xyxy(r, 0, W, H);
    }

    void on_change (void* what) override
    {
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
            refresh();

        if (what == &splitter1) refresh();
        if (what == &splitter2) refresh();

        if (what == &dicon 
        or  what == &conon
        or  what == &onetwo)
            refresh();
    }
};

sys::app<App> application ("en app");

#include "../auxs/windows_fonts.h"
#include "../auxs/windows_images.h"
#include "../auxs/windows_system.h"
#include "../auxs/windows_windows.h"