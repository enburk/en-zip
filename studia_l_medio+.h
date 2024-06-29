#pragma once
#include "studia_l_medio++.h"
namespace studia
{
    using namespace std::filesystem;

    struct Mediadetail:
    widget<Mediadetail>
    {
        gui::canvas canvas;
        gui::button button_full;
        gui::button button_crop;
        gui::button button_qrop;
        gui::button button_Qrop;
        gui::button button_drop;
        gui::button button_cmin;
        gui::button button_Zoom;
        gui::button button_zoom;
        gui::button button_play;
        gui::button button_stop;
        mediadetail full;
        mediadetail crop;
        mediadetail qrop;
        xy fullsize;
        xy cropsize;
        xy qropsize;

        int zoom = 100;
        str ZOOM = "studio::dic::detail::zoom";

        media::resource resource;
        media::Resource Resource;

        Mediadetail () { select({},{}); }
       ~Mediadetail () { select({},{}); }

        void select (path dat, path txt)
        {
            resource = media::resource(dat);
            Resource = media::Resource(txt);

            button_play.show();
            button_stop.hide();

            if (dat == path{} or not exists(dat))
            {
                full.load({},{},{});
                crop.load({},{},{});
                qrop.load({},{},{});
                return;
            }

            array<str> options;
            options += resource.options;
            options += Resource.options;

            str Crop, Qrop, Fade;
            for (str o: options) {
            if (o.starts_with("crop ")) Crop = o.from(5);
            if (o.starts_with("qrop ")) Qrop = o.from(5);
            if (o.starts_with("fade" )) Fade = o; }
            if (Qrop == "")
                Qrop = Crop;

            fullsize = full.load(dat, "",   ""  );
            cropsize = crop.load(dat, Crop, Fade);
            qropsize = qrop.load(dat, Qrop, Fade);
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*12/10;

                button_full.coord = xywh(0*W/9, 0, W/9, h);
                button_crop.coord = xywh(1*W/9, 0, W/9, h);
                button_qrop.coord = xywh(2*W/9, 0, W/9, h);
                button_Qrop.coord = xywh(3*W/9, 0, W/9, h);
                button_drop.coord = xywh(4*W/9, 0, W/9, h);
                button_cmin.coord = xywh(5*W/9, 0, W/9, h);
                button_Zoom.coord = xywh(6*W/9, 0, W/9, h);
                button_zoom.coord = xywh(7*W/9, 0, W/9, h);
                button_play.coord = xywh(8*W/9, 0, W/9, h);
                button_stop.coord = xywh(9*W/9, 0, W/9, h);

                canvas.coord = xyxy(0, h, W, H);
                full  .coord = xyxy(0, h, W, H);
                crop  .coord = xyxy(0, h, W, H);
                qrop  .coord = xyxy(0, h, W, H);
            }
            if (what == &skin)
            {
                auto s = gui::skins[skin];
                auto l = gui::metrics::line::width;

                canvas.color = s.ultralight.first;

                button_full.kind = gui::button::sticky;
                button_crop.kind = gui::button::sticky;
                button_qrop.kind = gui::button::sticky;

                button_full.text.text = "full";
                button_crop.text.text = "crop";
                button_qrop.text.text = "qrop";
                button_Qrop.text.text = "qropt!";
                button_drop.text.text = "reset";
                button_cmin.text.text = "course-";
                button_Zoom.text.text = "zoom+";
                button_zoom.text.text = "zoom-";
                button_play.text.text = "play";
                button_stop.text.text = "stop";
                button_full.on = true;

                zoom = sys::settings::load(ZOOM, 100);
                full.zoom = zoom;
                crop.zoom = zoom;
                qrop.zoom = zoom;
            }

            if (what == &button_Zoom)
            {
                zoom = min(100, zoom+10);
                sys::settings::save(ZOOM, zoom);
                full.zoom = zoom; full.resize();
                crop.zoom = zoom; crop.resize();
                qrop.zoom = zoom; qrop.resize();
            }
            if (what == &button_zoom)
            {
                zoom = max(10, zoom-10);
                sys::settings::save(ZOOM, zoom);
                full.zoom = zoom; full.resize();
                crop.zoom = zoom; crop.resize();
                qrop.zoom = zoom; qrop.resize();
            }
            if (what == &button_play)
            {
                if (button_full.on) full.play();
                if (button_crop.on) crop.play();
                if (button_qrop.on) qrop.play();
                button_play.hide();
                button_stop.show();
            }
            if (what == &button_stop)
            {
                if (button_full.on) full.stop();
                if (button_crop.on) crop.stop();
                if (button_qrop.on) qrop.stop();
                button_play.show();
                button_stop.hide();
            }
            if (what == &button_full)
            {
                button_crop.on = false;
                button_qrop.on = false;
                full.show();
                crop.hide();
                qrop.hide();
                full.play();
            }
            if (what == &button_crop)
            {
                button_full.on = false;
                button_qrop.on = false;
                full.hide();
                crop.show();
                qrop.hide();
                crop.play();
            }
            if (what == &button_qrop)
            {
                button_full.on = false;
                button_crop.on = false;
                full.hide();
                crop.hide();
                qrop.show();
                qrop.play();
            }
            if (what == &button_Qrop)
            {
                resource.options.
                try_emplace(
                "qropt!");
                notify();
            }
            if (what == &button_cmin)
            {
                resource.options.
                try_emplace(
                "course-");
                notify();
            }
            if (what == &button_drop)
            {
                str o = 
                button_full.on? "crop ":
                button_crop.on? "crop ":
                button_qrop.on? "qrop ": "";
                if (o == "") return;
                resource.options.erase_if([o](str x)
                { return x.starts_with(o); });
                notify();
            }
            if (what == &full) recrop("crop ", full.crop, fullsize, full.zoomsize, full.newcrop);
            if (what == &crop) recrop("crop ", crop.crop, cropsize, crop.zoomsize, crop.newcrop);
            if (what == &qrop) recrop("qrop ", qrop.crop, qropsize, qrop.zoomsize, qrop.newcrop);
        }

        void recrop (str kind, str oldcrop, xy newsize, xy zoomsize, xyxy newcrop)
        {
            xyxy r0 (0, 0, fullsize.x, fullsize.y);
            xyxy r1 = media::video::crop(fullsize, oldcrop);
            xyxy r2 = newcrop;

            int ww = xywh(r1).w;
            int hh = xywh(r1).h;

            r2.l = r2.l * ww/zoomsize.x;
            r2.r = r2.r * ww/zoomsize.x;
            r2.t = r2.t * hh/zoomsize.y;
            r2.b = r2.b * hh/zoomsize.y;
            
            r1.l = r1.l + r2.l;
            r1.t = r1.t + r2.t;
            r1.r = r1.l + r2.r - r2.l;
            r1.b = r1.t + r2.b - r2.t;
            
            int W = xywh(r0).w; if (W <= 0) return;
            int H = xywh(r0).h; if (H <= 0) return;

            int l = clamp<int>(100.0*(r1.xl - r0.xl)/W + 0.5);
            int t = clamp<int>(100.0*(r1.yl - r0.yl)/H + 0.5);
            int r = clamp<int>(100.0*(r0.xh - r1.xh)/W + 0.5);
            int b = clamp<int>(100.0*(r0.yh - r1.yh)/H + 0.5);

            int w = xywh(r1).w;
            int h = xywh(r1).h;

            bool q = 100*abs(w-h)/min(W,H) == 0;

            str crop = "";

            if (q and l == r and t == b) crop = "q"; else
            if (q and l == 0 and t == 0 and b == 0) crop = "ql"; else
            if (q and r == 0 and t == 0 and b == 0) crop = "qr"; else
            if (q and t == 0 and l == 0 and r == 0) crop = "qt"; else
            if (q and b == 0 and l == 0 and r == 0) crop = "qb"; else
            {
                if (l > 0) crop += " l" + std::to_string(l);
                if (t > 0) crop += " t" + std::to_string(t);
                if (r > 0) crop += " r" + std::to_string(r);
                if (b > 0) crop += " b" + std::to_string(b);

                crop.strip();
            }

            for (str& o: resource.options)
                if (o.starts_with(kind)) {
                    o = kind + crop;
                    notify();
                    return; }

            resource.options +=
            kind + crop;
            notify();
        }
    };
}
