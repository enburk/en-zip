#pragma once
#include "app_dict.h"
#include "app_dict_html.h"
#include "app_dict_media.h"
namespace app::dict::video
{
    struct player : gui::widget<player>
    {
        gui::canvas canvas;
        gui::frame  frame1;
        gui::frame  frame2;
        gui::player video;
        html_view   script;
        html_view   credit;
        gui::button prev;
        gui::button next;
        gui::property<gui::time> timer;
        mediae::media_index index;
        gui::media::state state;
        gui::time start, stay;

        player ()
        {
            auto font = credit.font.now;
            font.size = gui::metrics::text::height*5/6;
            credit.font = font;
            credit.alignment = XY{pix::right, pix::top};
            script.alignment = XY{pix::left,  pix::top};
            prev.text.text = (char*)(u8"\u25C0");
            next.text.text = (char*)(u8"\u25B6");
            prev.text.font = sys::font{"", gui::metrics::text::height/2};
            next.text.font = sys::font{"", gui::metrics::text::height/2};
            prev.frame.thickness = 0;
            next.frame.thickness = 0;
            on_change(&skin);
        }

        void reset (mediae::media_index index_)
        {
            start = gui::time{};
            stay  = gui::time{4000 +
                index_.title.size() * 90/1000 +
                index_.credit.size() * 30/1000 +
                index_.comment.size() * 10/1000
            };

            if (index == index_) return; else
                index =  index_;

            state = gui::media::state::loading;

            std::filesystem::path dir = "../data/app_dict";
            std::string storage = "storage." +
                std::to_string(index.location.source)
                + ".dat";

            video.load(dir / storage,
                index.location.offset,
                index.location.length);

            str c = index.credit;
            str s = index.title;

            s = ::app::dict::mediae::canonical(s);

            str date;
            for (str option : index.options)
                if (option.starts_with("date "))
                    date = option.from(5);

            if (date != "") c += ", <i>" + date + "</i>";

            if (index.comment != "") s += "<br><br>"
                "<font color=#808080><i>" + 
                ::app::dict::mediae::canonical(index.comment) +
                "</i></font><br>&nbsp;";

            script.it_is_a_title = true;
            script.html = s;
            credit.html = c;
        }

        void play ()
        {
            start = gui::time::now;
            state = gui::media::state::playing;
            stay.ms = stay.ms * 120/100;
            video.play();
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

            XY size {
                index.location.size_x,
                index.location.size_y};

            int maxwidth = width - 6*l;
            if (maxwidth < size.x) size = XY (
                maxwidth, maxwidth *
                size.y / size.x);

            credit.alignment = XY{pix::left, pix::top};

            script.coord = XYWH(0, 0, size.x, max<int>());
            credit.coord = XYWH(0, 0, size.x-3*d, max<int>());

            int w1 = script.view.column.coord.now.w;
            int w2 = credit.view.column.coord.now.w;

            credit.alignment = XY{pix::right, pix::top};

            int h1 = script.view.column.coord.now.h;
            int h2 = credit.view.column.coord.now.h;
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
            XYWH script_save = script.coord.now;
            XYWH credit_save = credit.coord.now;

            auto [w1, h1, w2, h2, size, w, h, d, y2] = sizes(width);

            script.coord = script_save;
            credit.coord = credit_save;

            return h;
        }

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go (gui::time::infinity,
                          gui::time::infinity);

            if (what == &coord)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;

                auto [w1, h1, w2, h2, size, w, h, d, y2] = sizes(coord.now.size.x);

                XYWH r (W/2 - w/2, H/2 - h/2, w, h);
                frame1.coord = r; r.deflate(frame1.thickness.now);
                frame2.coord = r; r.deflate(frame2.thickness.now);
                canvas.coord = r; r.deflate(frame2.thickness.now);

                video .coord = XYWH(r.x, r.y,  size.x, size.y);
                script.coord = XYWH(r.x, r.y + size.y, size.x, h1);
                credit.coord = XYWH(r.x + r.w - 3*d - w2, r.y + size.y + y2, w2, h2);
                prev  .coord = XYWH(r.x + r.w - 2*d, r.y + size.y + y2, d, d);
                next  .coord = XYWH(r.x + r.w - 1*d, r.y + size.y + y2, d, d);
            }

            if (what == &skin)
            {
                frame1.color = gui::skins[skin].ultralight.first;
                frame2.color = gui::skins[skin].normal.first;
                canvas.color = gui::skins[skin].light.first;
                credit.color = gui::skins[skin].hovered.first;
                script.color = gui::skins[skin].touched.first;
                int h = gui::metrics::text::height*11/10;
                auto font = sys::font{"", h};
                prev.text.font = font;
                next.text.font = font;
                prev.text.shift = XY{0, h/5};
                next.text.shift = XY{0, h/5};
                prev.text.color = gui::skins[skin].touched.first;
                next.text.color = gui::skins[skin].touched.first;
            }

            if (what == &timer)
            {
                using st = gui::media::state;

                if (state == st::loading)
                {
                    switch(video.state.load()) {
                    case st::failure:  state =
                         st::failure;  break;
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
                    switch(video.state.load()) {
                    case st::failure:  state =
                         st::failure;  break;
                    case st::finished: state = start + stay < gui::time::now ?
                         st::finished: st::playing; break;
                    default: break;
                    }
                }

                if (mouse_hover_child)
                    start = gui::time::now;
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &credit) { clicked = credit.clicked; notify(); }
            if (what == &script) { clicked = script.clicked; notify(); }

            if (what == &next) { clicked = -1; notify(); }
            if (what == &prev) { clicked = -2; notify(); }
        }
    };
}