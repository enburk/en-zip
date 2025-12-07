#pragma once
#include "studia_l_medio.h"
namespace studia
{
    struct Media:
    widget<Media>
    {
        gui::area<
        gui::selector> select;
        gui::area<detail> detail;
        gui::area<gui::console> played;
        gui::area<gui::console> audios;
        gui::area<gui::console> videos;
        gui::splitter splitter;
        int clicked = 0;

        Media ()
        {
            app::logs::media = played.object;
            app::logs::audio = audios.object;
            app::logs::video = videos.object;

            int i = 0;
            auto& sel = select.object;
            sel.buttons(i++).text.text = "played";
            sel.buttons(i++).text.text = "audios";
            sel.buttons(i++).text.text = "videos";
            sel.selected = 0;

            played.object.view.wordwrap = false;
            audios.object.view.wordwrap = false;
            videos.object.view.wordwrap = false;
            played.object.view.ellipsis = true;
            audios.object.view.ellipsis = true;
            videos.object.view.ellipsis = true;
            played.object.limit = 64*1024;
            audios.object.limit = 64*1024;
            videos.object.limit = 64*1024;
        }

        void on_change (void* what) override
        {
            if (what == &splitter
            or  what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*13/10;
                int x = splitter.set("studia::media::splitter", 25, 60, 75);

                detail.coord = xyxy(0, 0, x, H);
                select.coord = xyxy(x, 0, W, h);
                played.coord = xyxy(x, h, W, H);
                audios.coord = xyxy(x, h, W, H);
                videos.coord = xyxy(x, h, W, H);
            }

            if (what == &select)
            {
                int n = select.
                object.selected.now;
                played.show(n == 0);
                audios.show(n == 1);
                videos.show(n == 2);
            }

            if (what == &detail) {
                clicked = detail.object.
                clicked;
                notify();
            }

            str link;
            if (what == &played.object.link)
                link =   played.object.link;
            if (what == &audios.object.link)
                link =   audios.object.link;
            if (what == &videos.object.link)
                link =   videos.object.link;
            if (link.starts_with("file://"))
                detail.object.select(
                link.from(7));
        }
    };
}
