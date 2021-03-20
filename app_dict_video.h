#pragma once
#include "app_dict_html.h"
#include "app_dict_media.h"
namespace app::dict::video
{
    struct player : gui::widget<player>
    {
        gui::canvas canvas;
        gui::frame  frame1;
        gui::frame  frame2;
        gui::player image;
        html_view   credit;
        html_view   script;
        gui::button prev;
        gui::button next;
        gui::property<gui::time> timer;
        mediae::media_index index;

        player ()
        {
            auto font = credit.font.now;
            font.size = gui::metrics::text::height*5/6;
            credit.font = font;
            credit.alignment = XY{pix::right, pix::top};
            prev.text.text = (char*)(u8"\u25B2");//(u8"\u25C0");
            next.text.text = (char*)(u8"\u25BC");//(u8"\u25B6");
            prev.text.font = sys::font{"", gui::metrics::text::height/2};
            next.text.font = sys::font{"", gui::metrics::text::height/2};
            prev.frame.thickness = 0;
            next.frame.thickness = 0;
            on_change(&skin);
        }

        int height (int width)
        {
            int l = gui::metrics::line::width;
            int d = credit.font.now.size;

            credit.coord = XYWH(0, 0, width-2*d, max<int>());
            script.coord = XYWH(0, 0, width, max<int>());

            int h1 = credit.view.column.coord.now.size.y;
            int h2 = script.view.column.coord.now.size.y;

            XY size {index.location.size_x, index.location.size_y};
            int maxwidth = width - 6*l;
            if (maxwidth < size.x) size = XY (
                maxwidth, maxwidth *
                size.y / size.x);

            return size.y + 6*l + h1 + h2;
        }

        void reset (mediae::media_index index_)
        {
            index = index_;
            std::filesystem::path dir = "../data/app_dict";
            std::string storage = "storage." + std::to_string(index.location.source) + ".dat";
            image.load(dir / storage, index.location.offset, index.location.length);
            credit.html = index.credit;
            str s = index.title;
            if (index.comment != "")
                s += "<br><br>"
                "<font color=#EEEEEE><i>" + index.comment +
                "</i></font>";
            script.html = s;
        }

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go (gui::time::infinity,
                          gui::time::infinity);

            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int l = gui::metrics::line::width;
                int d = credit.font.now.size;

                XY size {index.location.size_x, index.location.size_y};
                int maxwidth = coord.now.size.x - 6*l;
                if (maxwidth < size.x) size = XY (
                    maxwidth, maxwidth *
                    size.y / size.x);

                credit.coord = XYWH(0, 0, size.x-2*d, H);
                script.coord = XYWH(0, 0, size.x, H);

                int h1 = credit.view.column.coord.now.size.y;
                int h2 = script.view.column.coord.now.size.y;

                int w = size.x + 6*l;
                int h = size.y + 6*l + h1 + h2;

                XYWH r (W/2 - w/2, H/2 - h/2, w, h);
                frame1.coord = r; r.deflate(frame1.thickness.now);
                frame2.coord = r; r.deflate(frame2.thickness.now);
                canvas.coord = r; r.deflate(frame2.thickness.now);

                image .coord = XYWH(r.x, r.y,  size.x, size.y);
                prev  .coord = XYWH(r.x+0*d, r.y + size.y, d, d);
                next  .coord = XYWH(r.x+1*d, r.y + size.y, d, d);
                credit.coord = XYWH(r.x+2*d, r.y + size.y, size.x-2*d, h1);
                script.coord = XYWH(r.x, r.y + size.y + h1, size.x, h2);
            }

            if (what == &skin)
            {
                frame1.color = gui::skins[skin].ultralight.first;
                frame2.color = gui::skins[skin].normal.first;
                canvas.color = gui::skins[skin].light.first;
                credit.color = gui::skins[skin].heavy.first;
            }

            if (what == &timer)
            {
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &credit) { clicked = credit.clicked; notify(); }
            if (what == &script) { clicked = script.clicked; notify(); }
        }
    };

    struct sequencer : gui::widget<sequencer>
    {
        gui::widgetarium<player> players;
        gui::property<int> current = 0;
        gui::property<gui::time> timer;

        void reset ()
        {
            int n = 0;
            for (auto index : mediae::selected_video)
                players(n++).reset(index);
            players.truncate(n);
            current = 0;

            for (auto & player : players) {
                player.prev.enabled = n > 1;
                player.next.enabled = n > 1;
            }
        }

        int height (int width)
        {
            int height = 0;
            for (auto & player : players)
                height = max (height, player.height(width));
            return height;
        }

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go (gui::time::infinity,
                          gui::time::infinity);

            if (what == &coord && coord.was.size != coord.now.size)
            {
                players.coord = coord.now.local();
                for (auto & player : players)
                    player.coord = coord.now.local();
            }

            if (what == &timer)
            {
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            clicked = players.notifier->clicked;
            notify();
        }
    };
}