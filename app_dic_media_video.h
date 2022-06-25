#pragma once
#include "app_dic_media_player.h"
namespace app::dic::video
{
    struct player:
    widget<player>
    {
        gui::canvas canvas;
        gui::frame  frame1;
        gui::frame  frame2;
        media::player video;
        html_view   script;
        html_view   credit;
        gui::button prev;
        gui::button next;
        gui::property<bool> mute = false;
        gui::property<gui::time> timer;
        media::media_index index;
        gui::media::state state =
        gui::media::state::finished;;
        gui::time start, stay;
        int clicked = 0;
        str error;

        void reset (str title,
            media::media_index video_index,
            media::media_index audio_index,
            array<str> links)
        {
            script.forbidden_links = links;
            credit.forbidden_links = links;

            if (index == video_index) return; else
                index =  video_index;

            start = gui::time{};
            stay  = gui::time{4000 +
                index.title.size() * 40 +
                index.credit.size() * 10 +
                index.comment.size() * 20
            };

            state = gui::media::state::loading;
            video.load(title, video_index, audio_index);

            str c = index.credit;
            str s = index.title;

            s = media::canonical(s);

            str date;
            for (str option : index.options)
                if (option.starts_with("date "))
                    date = option.from(5);

            if (date != "") c += ", <i>" + date + "</i>";

            if (index.comment != "") s += "<br><br>" +
                gray(italic(media::canonical(
                index.comment))) + "<br>&nbsp;";

            script.html = s;
            credit.html = c;
        }

        void play ()
        {
            start = gui::time::now;
            state = gui::media::state::playing;
            stay.ms = stay.ms * 120/100;
            video.play();

            logs::media << media::log(index);
        }
        void stop ()
        {
            video.stop();
        }

        auto sizes (int width)
        {
            int l = gui::metrics::line::width;
            int d = credit.font.now.size*4/3; if (d == 0)
                d = gui::metrics::text::height;

            xy size {
                index.location.size_x,
                index.location.size_y};

            int maxwidth = width - 6*l;
            if (maxwidth < size.x) size = xy (
                maxwidth, maxwidth *
                size.y / size.x);

            credit.alignment = xy{pix::left, pix::top};

            script.coord = xywh(0, 0, size.x, max<int>());
            credit.coord = xywh(0, 0, size.x-3*d, max<int>());

            int w1 = script.view.cell.coord.now.w;
            int w2 = credit.view.cell.coord.now.w;

            credit.alignment = xy{pix::right, pix::top};

            int h1 = script.view.cell.coord.now.h;
            int h2 = credit.view.cell.coord.now.h;
            int hh = h1 + h2;
            int y2 = h1;

            if (w1 + w2 + 3*d < size.x*9/10) {
                hh = max(h1, h2);
                y2 = 0;
            }

            int w = size.x + 6*l;
            int h = size.y + 6*l + hh;

            return std::tuple{w1, h1, w2, h2, size, w, h, d, y2};
        }

        int height (int width)
        {
            xywh script_save = script.coord.now;
            xywh credit_save = credit.coord.now;

            auto [w1, h1, w2, h2, size, w, h, d, y2] = sizes(width);

            script.coord = script_save;
            credit.coord = credit_save;

            return h;
        }

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go(gui::time::infinity,
                         gui::time::infinity);

            if (what == &coord)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;

                auto [w1, h1, w2, h2, size, w, h, d, y2] = sizes(coord.now.size.x);

                xywh r (W/2 - w/2, H/2 - h/2, w, h);
                frame1.coord = r; r.deflate(frame1.thickness.now);
                frame2.coord = r; r.deflate(frame2.thickness.now);
                canvas.coord = r; r.deflate(frame2.thickness.now);

                int l = d/7;
                prev.icon.padding = xyxy(l,2*l,l,2*l);
                next.icon.padding = xyxy(l,2*l,l,2*l);

                video .coord = xywh(r.x, r.y,  size.x, size.y);
                script.coord = xywh(r.x, r.y + size.y, size.x, h1);
                credit.coord = xywh(r.x + r.w - 3*d - w2, r.y + size.y + y2, w2, h2);
                prev  .coord = xywh(r.x + r.w - 2*d, r.y + size.y + y2, d, d);
                next  .coord = xywh(r.x + r.w - 1*d, r.y + size.y + y2, d, d);
            }

            if (what == &skin)
            {
                frame1.color = gui::skins[skin].ultralight.first;
                frame2.color = gui::skins[skin].normal.first;
                canvas.color = gui::skins[skin].light.first;
                credit.color = gui::skins[skin].hovered.first;
                script.color = gui::skins[skin].touched.first;

                int h = gui::metrics::text::height*8/10;
                auto font = pix::font{"", h};
                prev.text.font = font;
                next.text.font = font;
                prev.text.shift = xy{0, h/5};
                next.text.shift = xy{0, h/5};
                prev.text.color = gui::skins[skin].touched.first;
                next.text.color = gui::skins[skin].touched.first;
                prev.icon.load(assets["icon.chevron.left.double.black.128x128"]);
                next.icon.load(assets["icon.chevron.right.double.black.128x128"]);
                prev.frame.thickness = 0;
                next.frame.thickness = 0;

                font = credit.font.now;
                font.size = gui::metrics::text::height*5/6;
                credit.font = font;
                credit.alignment = xy{pix::right, pix::top};
                script.alignment = xy{pix::left,  pix::top};
            }

            if (what == &timer)
            {
                using st = gui::media::state;

                if (state == st::loading)
                {
                    switch(video.state()) {
                    case st::failure:  state =
                         st::failure;  error = video.error(); break;
                    case st::ready:    state =
                         st::ready;    break;
                    case st::playing:  state =
                         st::playing;  break;
                    case st::finished: state =
                         st::playing;  break;
                    default: break;
                    }
                }

                if (state == st::playing)
                {
                    switch(video.state()) {
                    case st::failure:  state =
                         st::failure;  error = video.error();  break;
                    case st::finished: state = start + stay < gui::time::now ?
                         st::finished: st::playing; break;
                    default: break;
                    }
                }

                if (mouse_hover_child)
                    start = gui::time::now;
            }

            if (what == &credit) { clicked = credit.clicked; notify(); }
            if (what == &script) { clicked = script.clicked; notify(); }

            if (what == &next) { clicked = -1; notify(); }
            if (what == &prev) { clicked = -2; notify(); }

            if (what == &mute) video.mute = mute.now;
        }
    };
}