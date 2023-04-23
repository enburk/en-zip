#pragma once
#include "app_dic.h"
#include "app_one.h"
#include "app_two.h"
#include "app_contents.h"
#include "app_contents+.h"

struct App:
widget<App>
{
    gui::canvas canvas;
    gui::text::view where;
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
    gui::button Conon;
    gui::button trans;

    sfx::media::playback play;
    gui::button slow;
    gui::button fast;
    gui::button speed;
    gui::button mute;

    gui::console report;
    gui::console errors;

    App ()
    {
        skin = "gray+";
        canvas.color =
        gui::skins[skin].ultralight.first;
        where.alignment = xy{pix::left, pix::center};
        dicon.kind = gui::button::toggle;
        conon.kind = gui::button::toggle;
        Conon.kind = gui::button::toggle;
        trans.kind = gui::button::toggle;
        trans.on = sys::settings::load("app::rus", 0);
        conon.text.text = "\xE2""\x98""\xB0"; // Trigram For Heaven
        Conon.text.text = "\xE2""\x98""\xB7"; // Trigram For Earth
        dicon.text.text = "dictionary";
        trans.text.text = "rus";
        slow .text.text = "slower";
        fast .text.text = "faster";
        speed.text.text = sys::settings::load("app::speed", "1.0");
        app::speed = std::stof(speed.text.text);
        mute .text.text = "mute";
        mute .kind = gui::button::toggle;
        slow.repeat_delay = 0ms;
        fast.repeat_delay = 0ms;
        slow.repeat_lapse = 16ms;
        fast.repeat_lapse = 16ms;
        slow.repeating = true;
        fast.repeating = true;
        speed.enabled = false;
        onetwo.buttons(0).text.text = "course";
        onetwo.buttons(1).text.text = "catalogs";
        onetwo.maxwidth = max<int>();
        onetwo.selected = 0;
        ones.reload("app::ones", app::one::course.root);
        twos.reload("app::twos", app::two::course.root);
        Ones.reload("app::Ones", app::one::course.root);
        Twos.reload("app::Twos", app::two::course.root);
    }

    void reload () try
    {
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

        slow  .coord = xywh(l+0*w+0*v, H-h+d, 2*v, h-d-d);
        speed .coord = xywh(l+0*w+2*v, H-h+d, 1*v, h-d-d);
        fast  .coord = xywh(l+0*w+3*v, H-h+d, 2*v, h-d-d);
        mute  .coord = xywh(l+0*w+5*v, H-h+d, 2*v, h-d-d);

        play  .coord = xywh(l+0*w+8*v, H-h+d, 2*w, h-d-d);
        play.enabled = onetwo.selected.now == 0
        and not Conon.on.now;

        conon .coord = xywh(r-3*w-3*v, H-h+d, 1*v, h-d-d);
        Conon .coord = xywh(r-3*w-2*v, H-h+d, 1*v, h-d-d);
        trans .coord = xywh(r-3*w-1*v, H-h+d, 1*v, h-d-d);
        onetwo.coord = xywh(r-3*w-0*v, H-h+d, 2*w, h-d-d);
        dicon .coord = xywh(r-1*w-0*v, H-h+d, 1*w, h-d-d);
        where .coord = xywh(r+0*w+1*v, H-h+d, 9*w, h-d-d);

        if (not conon.on.now) l = 0;
        if (not dicon.on.now) r = W;

        one .show(onetwo.selected.now == 0);
        two .show(onetwo.selected.now == 1);
        ones.show(onetwo.selected.now == 0 and conon.on.now);
        twos.show(onetwo.selected.now == 1 and conon.on.now);
        Ones.show(onetwo.selected.now == 0 and Conon.on.now);
        Twos.show(onetwo.selected.now == 1 and Conon.on.now);
        dic .show(dicon.on.now);

        splitter1.show(conon.on.now);
        splitter2.show(dicon.on.now);

        canvas.coord = xyxy(0, 0, W, H);
        ones  .coord = xyxy(0, 0, l, H);
        twos  .coord = xyxy(0, 0, l, H);
        Ones  .coord = xyxy(l, 0, r, H-h);
        Twos  .coord = xyxy(l, 0, r, H-h);
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
        or  what == &Conon
        or  what == &onetwo)
            refresh();

        if (what == &play.play) one.play();
        if (what == &play.stop) one.stop();
        if (what == &play.next) one.next();
        if (what == &play.prev) one.prev();
        if (what == &play.Next) one.Next();
        if (what == &play.Prev) one.Prev();

        if (what == &ones) one.go(ones.selected);
        if (what == &Ones) one.go(Ones.selected);

        if (what == &ones
        or  what == &Ones
        or  what == &one)
            where.html =
            one.where;

        if (what == &trans)
        {
            sys::settings::save("app::rus",
                trans.on? 1:0);
            one.translated =
                trans.on;
        }

        if (what == &slow)
        {
            str s = speed.text.text;
            double k = std::stof(s);
            if (k > 0.19) k -= 0.1,
            app::speed = k,
            speed.text.text =
            std::format("{:1.1f}",k);
            sys::settings::save("app::speed",
            speed.text.text);
        }
        if (what == &fast)
        {
            str s = speed.text.text;
            double k = std::stof(s);
            if (k < 9.81) k += 0.1,
            app::speed = k,
            speed.text.text =
            std::format("{:1.1f}",k);
            sys::settings::save("app::speed",
            speed.text.text);
        }

        if (what == &mute)
        {
            one.mute = mute.on;
            dic.left.quot.object.mute.on =
            mute.on;
        }
    }

    bool mouse_sensible (xy) override { return true; }
};
