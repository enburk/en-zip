#pragma once
#include "app_dic.h"
namespace studio::dic
{
    using namespace std::filesystem;

    struct mediadetail:
    widget<mediadetail>
    {
        sfx::media::player player;

        gui::Frame cropper;
        gui::timer loading;

        gui::text::view error;

        path source;
        str crop, fade;
        int zoom  = 100;
        int scale = 100;
        int ratio = 50;
        xyxy newcrop;
        xy zoomsize;

        mediadetail () { load({},{},{}); }
       ~mediadetail () { load({},{},{}); }

        xy load (path source, str crop, str fade) try
        {
            this->source = source;
            this->crop = crop;
            this->fade = fade;
            error.hide();

            scale = 100;
            ratio = 50;

            player.load({},{});

            if (source == path{}) return {};

            str stem = str(source.stem());
            if (crop != "") stem += " == crop " + crop;
            if (fade != "") stem += " == fade " + fade;

            str ext = source.extension().string();
            ext = ext.ascii_lowercased();
            if (media::audioexts.
                contains(ext))
                ext = ".ogg";

            path
            cache = "../data/!detail";
            cache /= stem + ext;

            array<byte> video_bytes;
            array<byte> audio_bytes;
            if (media::videoexts.contains(ext))
            video_bytes = media::video::readsample(source, cache, crop).value(); else
            audio_bytes = media::audio::readsample(source, cache, crop, fade).value();

            xy size;
            if (media::videoexts.contains(ext))
            if (auto x = pix::size(video_bytes.from(0)); x.ok())
                size = x.value();

            player.load(
            std::move(video_bytes),
            std::move(audio_bytes));

            loading.start();

            return size;
        }
        catch (std::exception const& e)
        {
            error.html =
            str(source) + "<br>" +
            red(bold(aux::unicode::
            what(e.what())));
            error.show();
            return xy{};
        }

        void play () { player.play(); }
        void stop () { player.stop(); }

        void resize ()
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

            error.coord =
            coord.now.local();
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
                resize();

            if (what == &loading
            and player.status != sfx::media::state::loading)
            {
                bool video =
                media::videoexts.contains(
                source.extension().string());
                if (video) player.play();
                loading.stop();
                resize();
            }
        }

        bool mouse_sensible (xy  ) override { return true; }
        void on_mouse_hover (xy p) override
        {
            cropper.hide();

            newcrop = xyxy{};

            xyxy R = player.coord.now;

            if (not R.includes(p)) return;

            int W = xywh(R).w;
            int H = xywh(R).h;

            int w = min(W, min(W,H)*scale*(    ratio)/100/50);
            int h = min(H, min(W,H)*scale*(100-ratio)/100/50);

            xyxy r;
            r.xl = max(R.xl, min(R.xh-w, p.x-w/2)); r.xh = r.xl + w;
            r.yl = max(R.yl, min(R.yh-h, p.y-h/2)); r.yh = r.yl + h;

            newcrop = r - R.lt;
            zoomsize = xy(W,H);
            r.inflate(2);

            cropper.frame1.color = pix::ARGB(0xAA000000);
            cropper.frame2.color = pix::ARGB(0xAAFFFF00);
            cropper.frame3.color = pix::ARGB(0xAA000000);
            cropper.coord = r;
            cropper.show();
        }
        void on_mouse_leave () override
        {
            cropper.hide();
        }
        void on_mouse_click (xy, str button, bool down) override
        {
            if (button == "left" and down and newcrop != xyxy{})
            {
                notify();
            }
        }
        bool on_mouse_wheel (xy p, int delta) override
        {
            delta /=
            abs(delta) < 100?
            abs(delta) : 100;
            if (sys::keyboard::ctrl)
            ratio = max(10, min( 90, ratio - delta)); else
            scale = max(10, min(400, scale + delta));
            return true;
        }
    };
}
