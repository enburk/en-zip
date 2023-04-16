#pragma once
#include "app_dic.h"
namespace studio::dic
{
    using namespace std::filesystem;

    struct mediadetail:
    widget<mediadetail>
    {
        media::resource resource;
        media::Resource Resource;

        gui::canvas canvas;
        gui::radio::group crops;
        gui::button button_play;
        gui::button button_stop;
        gui::button button_Zoom;
        gui::button button_zoom;
        gui::button button_full;
        gui::button button_crop;
        gui::button button_qrop;
        sfx::media::player full;
        sfx::media::player crop;
        sfx::media::player qrop;
        gui::button button_decrop;
        gui::button button_deqrop;

        gui::frame cropper1;
        gui::frame cropper2;
        gui::frame cropper3;

        gui::timer loading;
        gui::timer editing;

        int zoom = 100;
        str ZOOM = "studio::dic::detail::zoom";

        mediadetail () { select(path{}, path{}); }
       ~mediadetail () { select(path{}, path{}); }

        void select (path dat, path txt)
        {
            resource = media::resource(dat);
            Resource = media::Resource(txt);

            button_play.show();
            button_stop.hide();
            button_play.enabled = false;
            button_stop.enabled = false;
            button_full.enabled = false;
            button_crop.enabled = false;
            button_qrop.enabled = false;
            button_decrop.hide();
            button_deqrop.hide();

            full.load({},{});
            crop.load({},{});
            qrop.load({},{});

            if (dat == path{}) return;

            auto load = [](sfx::media::player& player, path source, str crop, str fade)
            {
                str stem = source.stem().string();
                str ext = source.extension().string();

                if (crop != "") stem += " == crop " + crop;
                if (fade != "") stem += " == fade " + fade;

                path
                cache = "../data/!detail";
                cache /= stem + ext;

                ext = ext.ascii_lowercased();
                if (media::audioexts.contains(ext))
                cache.replace_extension(".ogg");

                array<byte> video_bytes;
                array<byte> audio_bytes;
                if (media::videoexts.contains(ext))
                video_bytes = media::video::readsample(source, cache, crop).value(); else
                audio_bytes = media::audio::readsample(source, cache, crop, fade).value();

                player.load(
                std::move(video_bytes),
                std::move(audio_bytes));
            };

            array<str> options;
            options += resource.options;
            options += Resource.options;

            str ocrop, oqrop, ofade;
            for (str o: options) {
            if (o.starts_with("crop ")) ocrop = o.from(5);
            if (o.starts_with("qrop ")) oqrop = o.from(5);
            if (o.starts_with("fade ")) ofade = o.from(5);
            }

            bool cropped = ocrop != "" or ofade != "";
            bool qropped = oqrop != "";

            load(full, dat, "",    ""   ); if (cropped)
            load(crop, dat, ocrop, ofade); if (qropped)
            load(qrop, dat, oqrop, ofade);

            button_play.enabled = true;
            button_stop.enabled = true;
            button_full.enabled = true;
            button_crop.enabled = cropped;
            button_qrop.enabled = qropped;
            button_full.on = true;

            loading.start();
        }

        void refresh ()
        {
            auto resize = [this](sfx::media::player& player)
            {
                int y = 0;
                int x = 0;

                xy size = player.resolution;
                int maxsizex = coord.now.w;
                int maxsizey = coord.now.h;

                int sizex = maxsizex * zoom / 100;
                int sizey = maxsizey * zoom / 100;

                double k = sqrt ((double)(sizex*sizey) / (size.x*size.y));

                double kx = (double) size.x / sizex;
                double ky = (double) size.y / sizey;

                double fsizex = size.x * k;  int isizex = int (fsizex + 0.5);
                double fsizey = size.y * k;  int isizey = int (fsizey + 0.5);

                if (isizex > sizex
                or  isizey > sizey) // downscale
                {
                    if (kx > ky) { size = sizex * size / size.x; k = 1/kx; }
                    else         { size = sizey * size / size.y; k = 1/ky; }
                }

                x += (maxsizex - size.x) / 2;
                y += (maxsizey - size.y) / 2;

                player.coord = xywh(x, y,
                size.x, size.y);
            };
            resize(full);
            resize(crop);
            resize(qrop);
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

                button_full.coord = xywh(0*W/6, 0, W/6, h);
                button_crop.coord = xywh(1*W/6, 0, W/6, h);
                button_qrop.coord = xywh(2*W/6, 0, W/6, h);
                button_Zoom.coord = xywh(3*W/6, 0, W/6, h);
                button_zoom.coord = xywh(4*W/6, 0, W/6, h);
                button_play.coord = xywh(5*W/6, 0, W/6, h);
                button_stop.coord = xywh(5*W/6, 0, W/6, h);

                canvas.coord = xyxy(0, h, W, H);

                refresh();
            }
            if (what == &skin)
            {
                auto s = gui::skins[skin];
                auto l = gui::metrics::line::width;

                canvas.color = s.ultralight.first;

                button_full.kind = gui::button::sticky;
                button_crop.kind = gui::button::sticky;
                button_qrop.kind = gui::button::sticky;

                button_play.text.text = "play";
                button_stop.text.text = "stop";
                button_Zoom.text.text = "zoom+";
                button_zoom.text.text = "zoom-";
                button_full.text.text = "full";
                button_crop.text.text = "crop";
                button_qrop.text.text = "qrop";

                button_decrop.text.html = red("reset");
                button_deqrop.text.html = red("reset");

                zoom = sys::settings::load(
                ZOOM, 100);
            }

            if (what == &loading
            and full.status != sfx::media::state::loading
            and crop.status != sfx::media::state::loading
            and qrop.status != sfx::media::state::loading)
            {
                bool video =
                media::videoexts.contains(
                resource.path.extension().string());
                if (video and button_full.on) full.play();
                if (video and button_crop.on) crop.play();
                if (video and button_qrop.on) qrop.play();
                if (video) button_play.hide();
                if (video) button_stop.show();
                loading.stop();
                refresh();
            }
            if (what == &button_Zoom)
            {
                zoom = min(100, zoom+10);
                sys::settings::save(ZOOM, zoom);
                refresh();
            }
            if (what == &button_zoom)
            {
                zoom = max(10, zoom-10);
                sys::settings::save(ZOOM, zoom);
                refresh();
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
        }

        bool mouse_sensible (xy  ) override { return true; }
        void on_mouse_hover (xy p) override
        {
            cropper1.hide();
            cropper2.hide();
            cropper3.hide();

            xyxy r = 
            button_full.on? full.coord.now:
            button_crop.on? crop.coord.now:
            button_qrop.on? qrop.coord.now:
            xywh{};

            if (not r.includes(p)) return;

            int w = xywh(r).w;
            int h = xywh(r).h;

            if (w < h) { r.yl = max(r.yl, min(r.yh-w, p.y-w/2)); r.yh = r.yl + w; }
            if (w > h) { r.xl = max(r.xl, min(r.xh-h, p.x-h/2)); r.xh = r.xl + h; }

            r.inflate(2);

            cropper1.color = pix::ARGB(0x80444444);
            cropper2.color = pix::ARGB(0x80FFFF00);
            cropper3.color = pix::ARGB(0x80444444);
            cropper1.coord = r; r.deflate(1);
            cropper2.coord = r; r.deflate(1);
            cropper3.coord = r; r.deflate(1);
            cropper1.show();
            cropper2.show();
            cropper3.show();
        }
        void on_mouse_leave () override
        {
            cropper1.hide();
            cropper2.hide();
            cropper3.hide();
        }
    };
}
