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
    app::one::view one;
    app::two::view two;
    app::dic::view dic;
    app::contents ones;
    app::contents twos;
    app::Contents Ones;
    app::Contents Twos;
    gui::splitter splitter1;
    gui::splitter splitter2;
    gui::selector apps;
    gui::button dicon;
    gui::button conon;
    gui::button Conon;
    gui::button trans;

    sfx::media::Playback play;
    widgetarium<gui::button> levels;
    gui::button mode;
    gui::button slow;
    gui::button fast;
    gui::button speed;
    gui::button mute;
    gui::button sort;
    gui::button shuff;

    gui::console report;
    gui::console errors;

    bool first_time = true;

    App ()
    {
        #ifndef STANDALONE_APP
        hide(); one.played = true;
        #endif

        skin = "gray+";
        canvas.color =
        gui::skins[skin].ultralight.first;
        where.alignment = xy{pix::left, pix::center};
        dicon.kind = gui::button::toggle;
        conon.kind = gui::button::toggle;
        Conon.kind = gui::button::toggle;
        trans.kind = gui::button::toggle;
        mode .kind = gui::button::toggle;
        mute .kind = gui::button::toggle;

        trans.on = sys::settings::load("app::rus",  0);
        mute .on = sys::settings::load("app::mute", 0);
        mode .on = sys::settings::load("app::mode", 1);
        speed.text.text = sys::settings::load("app::speed", "1.0");

        conon.text.text = "\xE2""\x98""\xB0"; // Trigram For Heaven
        Conon.text.text = "\xE2""\x98""\xB7"; // Trigram For Earth
        dicon.text.text = "dictionary";
        trans.text.text = "rus";
        slow .text.text = "slower";
        fast .text.text = "faster";
        mode .text.text = "playing";
        mute .text.text = "mute";
        sort .text.text = "sort";
        shuff.text.text = "shuffle";
        slow.repeat_delay = 0ms;
        fast.repeat_delay = 0ms;
        slow.repeat_lapse = 16ms;
        fast.repeat_lapse = 16ms;
        speed.enabled = false;
        apps.buttons(0).text.text = "course";
        apps.buttons(1).text.text = "catalog";
        apps.selected = sys::settings::load("app::app", "course") == "course" ? 0 : 1;
        ones.reload("app::ones", app::one::course.root);
        twos.reload("app::twos", app::two::course.root);
        Ones.reload("app::Ones", app::one::course.root);
        Twos.reload("app::Twos", app::two::course.root);

        for (int i=0; i<6; i++)
        levels(i).kind = gui::button::toggle,
        levels(i).text.text = std::to_string(i+1),
        levels(i).on = sys::settings::load("app::level " +
            std::to_string(i+1), "on") == "on";
    }

    void one_reload () 
    {
        ones.reload("app::ones", app::one::course.root);
        Ones.reload("app::Ones", app::one::course.root);
        one.reload();
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
        int w = gui::metrics::text::height*4;
        int v = gui::metrics::text::height*5/3;
        int h = gui::metrics::text::height*13/10;
        int d = gui::metrics::line::width*2;
        int l = splitter1.set("app::splitter1",  7,  9, 11);
        int r = splitter2.set("app::splitter2", 50, 60, 70);

     // mode  .coord = xywh(l+0*w+ 0*v, H-h+d, 2*v, h-d-d);
        slow  .coord = xywh(l+0*w+ 2*v, H-h+d, 2*v, h-d-d);
        speed .coord = xywh(l+0*w+ 4*v, H-h+d, 1*v, h-d-d);
        fast  .coord = xywh(l+0*w+ 5*v, H-h+d, 2*v, h-d-d);
        mute  .coord = xywh(l+0*w+ 7*v, H-h+d, 2*v, h-d-d);
        sort  .coord = xywh(l+0*w+10*v, H-h+d, 2*v, h-d-d);
        shuff .coord = xywh(l+0*w+12*v, H-h+d, 2*v, h-d-d);

        play  .coord = xywh(r-5*w- 4*v, H-h+d, 6*v, h-d-d);
        levels.coord = xywh(r-5*w- 4*v, H-h+d, 5*v, h-d-d);

        conon .coord = xywh(r-3*w- 3*v, H-h+d, 1*v, h-d-d);
        Conon .coord = xywh(r-3*w- 2*v, H-h+d, 1*v, h-d-d);
        trans .coord = xywh(r-3*w- 1*v, H-h+d, 1*v, h-d-d);
        apps  .coord = xywh(r-3*w- 0*v, H-h+d, 2*w, h-d-d);
        dicon .coord = xywh(r-1*w- 0*v, H-h+d, 1*w, h-d-d);
        where .coord = xywh(r+0*w+ 1*v, H-h+d, 9*w, h-d-d);

        if (not conon.on.now) l = 0;
        if (not dicon.on.now) r = W;

        one .hide(apps.selected.now != 0 or  Conon.on.now);
        two .hide(apps.selected.now != 1 or  Conon.on.now);
        ones.show(apps.selected.now == 0 and conon.on.now);
        twos.show(apps.selected.now == 1 and conon.on.now);
        Ones.show(apps.selected.now == 0 and Conon.on.now);
        Twos.show(apps.selected.now == 1 and Conon.on.now);
        dic .show(dicon.on.now);

        play  .show(apps.selected.now == 0);
        levels.show(apps.selected.now == 1);

        play.enabled = one.shown();

        for (int i=0; i<levels.size(); i++)
        levels(i).enabled = two.shown(),
        levels(i).coord = xywh(
        levels.coord.now.w/levels.size()*i, 0,
        levels.coord.now.w/levels.size(),
        levels.coord.now.h);

        where.html = apps.selected.now == 0 ?
            one.where :
            two.where;

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
        if (what == &skin)
        {
            speed.on_change_state = [this]()
            {
                auto style = gui::skins[speed.skin.now];
                speed.canvas.color = style.dark.second;
                speed.text.color = style.dark.first;
                auto r =
                speed.coord.now.local();
                speed.frame.thickness = 0;
                speed.frame.coord = xywh{};
                speed.image.coord = xywh{};
                speed.text.coord = r; r.deflate(1);
                speed.canvas.coord = r;
            };
            speed.on_change_state();
        }

        if (what == &apps)
            sys::settings::save(
            "app::app", apps.selected.now == 0 ?
            "course" : "catalog");

        if (what == &alpha
        and alpha.to == 255
        and first_time)
            first_time = false,
            one.reload(),
            two.reload();

        if (what == &alpha
        and alpha.to  == 255
        and alpha.now == 255)
        {
            if (one.shown()) one.start();
            if (two.shown()) two.start();
        }

        if (what == &alpha
        and alpha.to  == 0
        and alpha.now == 0)
        {
            if (one.shown()) one.halt();
            if (two.shown()) two.halt();
        }

        if (what == &play.play) mode.on = true;
        if (what == &play.Stop) mode.on = false;

        if (what == &play.play) one.play();
        if (what == &play.stop) one.stop();
        if (what == &play.Stop) one.stop();
        if (what == &play.next) one.next();
        if (what == &play.prev) one.prev();
        if (what == &play.Next) one.Next();
        if (what == &play.Prev) one.Prev();

        if (what == &levels)
        {
            int idx = levels.notifier_index;
            bool on = levels.notifier->on.now;
            sys::settings::save("app::level " +
            std::to_string(idx+1), on ? "on" : "off");
            two.on(idx, on);
        }

        if (what == &ones) one.go(ones.selected);
        if (what == &Ones) one.go(Ones.selected);

        if (what == &one.status)
            play.status =
            one.status;

        if (what == &twos) two.go(twos.selected);
        if (what == &Twos) two.go(Twos.selected);

        if (what == &Ones) Conon.on = false;
        if (what == &Twos) Conon.on = false;

        if (what == &trans)
        {
            sys::settings::save(
            "app::rus", trans.on? 1:0);
            one.translated = trans.on;
            two.translated = trans.on;
        }

        if (what == &slow)
        {
            str s = speed.text.text;
            double k = std::stof(s) - 0.1;
            if (k < 0.5) k = 0.5;
            speed.text.text =
            std::format("{:1.1f}", k);
        }
        if (what == &fast)
        {
            str s = speed.text.text;
            double k = std::stof(s) + 0.1;
            if (k > 2.0) k = 2.0;
            speed.text.text =
            std::format("{:1.1f}", k);
        }
        if (what == &speed.text.update_text)
        {
            app::speed = std::stof(speed.text.text);
            sys::settings::save("app::speed",
            speed.text.text);
            one.speedup();
        }

        if (what == &mode)
        {
            one.playmode = mode.on;
            sys::settings::save("app::mode",
            mode.on? 1:0);
        }

        if (what == &mute)
        {
            one.mute = mute.on;
            dic.left.quot.object.mute.on = mute.on;
            sys::settings::save("app::mute",
            mute.on? 1:0);
        }

        if (what == &sort ) app::one::course.root.sort();
        if (what == &shuff) app::one::course.root.shuffle();
        if (what == &sort ) one_reload();
        if (what == &shuff) one_reload();

        int clicked = -1;
        if (what == &one) clicked = one.clicked;
        if (what == &two) clicked = two.clicked;
        if (clicked >= 0)
        {
            if (dicon.on and dic.clicked == clicked)
                dicon.on = false; else
                dicon.on = true, // order...:
                dic.go(clicked); // important
        }

        refresh();
    }

    bool mouse_sensible (xy) override { return true; }
    bool on_mouse_wheel (xy p, int delta) override
    {
        if (slow .coord.now.includes(p)
        or  fast .coord.now.includes(p)
        or  speed.coord.now.includes(p))
        {
            str s = speed.text.text;
            double k = std::stof(s);
            k += 0.1 * delta/120;
            if (k < 0.5) k = 0.5;
            if (k > 2.0) k = 2.0;
            speed.text.text =
            std::format("{:1.1f}", k);
            return true;
        }
        return false;
    }
};
