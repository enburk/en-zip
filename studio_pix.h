#pragma once
#include "app.h"
namespace studio::pixer
{
    struct studio:
    widget<studio>
    {
        gui::canvas canvas;
        gui::area<gui::text::one_line_editor> edit;
        gui::button button_sec;
        gui::button button_pix;
        gui::button button_png;
        gui::button button_jpg;
        gui::button button_l;
        gui::button button_t;
        gui::button button_r;
        gui::button button_b;
        gui::Image  Image;
        image<rgba> image;
        gui::Frame  frame;

        studio ()
        {
            button_sec.text.text = "shot in 5 sec";
            button_pix.text.text = "shot!";
            button_png.text.text = "png";
            button_jpg.text.text = "jpg";
            button_l  .text.text = "l";
            button_t  .text.text = "t";
            button_r  .text.text = "r";
            button_b  .text.text = "b";

            button_l.repeat_lapse = 40ms;
            button_t.repeat_lapse = 40ms;
            button_r.repeat_lapse = 40ms;
            button_b.repeat_lapse = 40ms;

            edit.show_focus = true;

            frame.frame1.color = pix::ARGB(0xAA000000);
            frame.frame2.color = pix::ARGB(0xAAFFFF00);
            frame.frame3.color = pix::ARGB(0xAA000000);
        }

        void resize ()
        {
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int w = gui::metrics::text::height*7;
            int h = gui::metrics::text::height*13/10;
            int v = w/2;

            canvas.coord = xywh(0, 0, W, H);

            button_sec.coord = xywh(3*w + 0*v, 0, w, h);
            button_pix.coord = xywh(4*w + 0*v, 0, w, h);
            button_l  .coord = xywh(5*w + 0*v, 0, v, h);
            button_t  .coord = xywh(5*w + 1*v, 0, v, h);
            button_r  .coord = xywh(6*w + 0*v, 0, v, h);
            button_b  .coord = xywh(6*w + 1*v, 0, v, h);
            button_png.coord = xywh(7*w + 0*v, 0, w, h);
            button_jpg.coord = xywh(8*w + 0*v, 0, w, h);
            edit      .coord = xyxy(9*w + 0*v, 0, W, h);

            int y = 0;
            int x = 0;

            xy size =
            Image.source.now.size;
            int maxsizex = coord.now.w;
            int maxsizey = coord.now.h - h;

            int sizex = min(maxsizex, size.x); if (sizex <= 0) return;
            int sizey = min(maxsizey, size.y); if (sizey <= 0) return;

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

            Image.coord =
            xywh(x, y + h,
            size.x,
            size.y);
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
                resize();

            if (what == &skin)
            {
                canvas.color =
                gui::skins[skin].
                ultralight.first;
            }

            if (what == &button_sec)
            {
                what =  &button_pix,
                std::this_thread::sleep_for(5s);
            }
            if (what == &button_pix)
            {
                sys::app_instance::app->hide();
                std::this_thread::sleep_for(200ms);
                image = sys::screen::snapshot();
                Image.source = image.crop();
                sys::app_instance::app->show();
                resize();
            }
            
            if (what == &button_l) crop = xyxy(Image.source.now), crop.l++;
            if (what == &button_t) crop = xyxy(Image.source.now), crop.t++;
            if (what == &button_r) crop = xyxy(Image.source.now), crop.r--;
            if (what == &button_b) crop = xyxy(Image.source.now), crop.b--;
            if (what == &button_l
            or  what == &button_t
            or  what == &button_r
            or  what == &button_b)
            {
                Image.source =
                image.crop(crop);
                resize();
            }

            if (what == &button_jpg)
            {
                crop = xyxy(
                Image.source.now);
                std::filesystem::
                path dir = "../datae";
                str fn = edit.object.text; fn.strip();
                if (fn != "") pix::write(
                image.crop(crop), dir/
                str2path(fn + ".jpg"));
            }
            if (what == &button_png)
            {
                crop = xyxy(
                Image.source.now);
                std::filesystem::
                path dir = "../datae";
                str fn = edit.object.text; fn.strip();
                if (fn != "") pix::write(
                image.crop(crop), dir/
                str2path(fn + ".png"));
            }
        }

        xy   p0;
        int  threshold = 10;
        bool clicked = false;
        xyxy crop;

        bool mouse_sensible (xy) override { return true; }
        void on_mouse_hover (xy p1) override
        {
            if (clicked)
            {
                frame.coord = xyxy{p0,p1};
                frame.show();
            }
            else if (p0 == xy{})
            {
                int  w = image.size.x;
                int  h = image.size.y;
                xywh R = Image.coord.now;
                if (not R.includes(p1))
                    return;

                xy p = p1 - R.origin;
                p.x = p.x * w / R.w;
                p.y = p.y * h / R.h;

                int ll = 0; rgba lcolor; int l = 0;
                int tt = 0; rgba tcolor; int t = 0;
                int rr = 0; rgba rcolor; int r = 0;
                int bb = 0; rgba bcolor; int b = 0;

                auto found = [this](int& n, rgba& color, int x, int y)
                {
                    rgba c = image(x,y);
                    if (c == color) n++;
                    else color = c, n = 0;
                    return n == threshold;
                };

                for (int x=p.x; x>0; x--) if (found(ll, lcolor, x, p.y)) { l = x+threshold; break; }
                for (int x=p.x; x<w; x++) if (found(rr, rcolor, x, p.y)) { r = x-threshold; break; }
                for (int y=p.y; y>0; y--) if (found(tt, tcolor, p.x, y)) { t = y+threshold; break; }
                for (int y=p.y; y<h; y++) if (found(bb, bcolor, p.x, y)) { b = y-threshold; break; }

                if (lcolor == rgba{}
                or  lcolor != rcolor
                or  rcolor != tcolor
                or  tcolor != bcolor)
                {
                    //l = p.x - 10;
                    //r = p.x + 10;
                    //t = p.y - 10;
                    //b = p.y + 10;
                    //return;
                }

                l--; t--;
                r++; b++;

                crop = xyxy{l, t, r, b};

                l = l * R.w / w + R.x;
                r = r * R.w / w + R.x;
                t = t * R.h / h + R.y;
                b = b * R.h / h + R.y;
                
                frame.coord = xyxy{l, t, r, b};
                frame.show();
            }
        }
        void on_mouse_leave () override
        {
            p0 = xy{};
            clicked = false;
            frame.hide();
        }
        void on_mouse_click (xy p1, str button, bool down) override
        {
            if (button != "left")
            return; clicked = down;

            if (down) p0 = p1; else
            if (frame.shown()
            and abs(p1.x-p0.x) < 9
            and abs(p1.y-p0.y) < 9)
            {
                Image.source =
                image.crop(crop);
                resize();
            }
            else
            {
                int  w = image.size.x;
                int  h = image.size.y;
                xywh R = Image.coord.now;
                if (not R.includes(p1))
                    return;

                p0 -= R.origin;
                p0.x = p0.x * w / R.w;
                p0.y = p0.y * h / R.h;

                p1 -= R.origin;
                p1.x = p1.x * w / R.w;
                p1.y = p1.y * h / R.h;

                Image.source =
                image.crop(xyxy(p0, p1));
                resize();
            }
        }
        bool on_mouse_wheel (xy p, int delta) override
        {
            delta /=
            abs(delta) < 120?
            abs(delta) : 120;
            threshold += delta;
            return true;
        }
    };
}
