#pragma once
#include "studia_dic_detail.h"
#include "studia_dic_search.h"
#include "studio_build_dic+.h"
namespace studio::dic
{
    struct area:
    widget<area>
    {
        gui::area<
        gui::selector> selector;
        gui::area<detail> detail;
        gui::area<search> search;

        gui::console log_medias; // 0
        gui::console log_audios; // 1
        gui::console log_videos; // 2
        gui::console log_errors; // 3
        gui::console log_usages; // 4
        gui::console log_search; // 5
        gui::console log_statts; // 6
        gui::console log_timing; // 7
        gui::splitter splitter;

        const int last_detail = 4;
        const int last_narrow = 5;
        const int inde_search = 5;

        array<gui::console*> consoles;

        int clicked = 0;

        area ()
        {
            consoles += &log_medias;
            consoles += &log_audios;
            consoles += &log_videos;
            consoles += &log_errors;
            consoles += &log_usages;
            consoles += &log_search;
            consoles += &log_statts;
            consoles += &log_timing;

            for (int i=1; i<
            consoles.size(); i++)
            consoles[i]->hide();

            search.object.result = &log_search;

            app::logs::media = log_medias;
            app::logs::audio = log_audios;
            app::logs::video = log_videos;
            app::logs::times = log_timing;

            int i = 0;
            auto& select = selector.object;
            select.buttons(i++).text.text = "media";
            select.buttons(i++).text.text = "audio";
            select.buttons(i++).text.text = "video";
            select.buttons(i++).text.text = "errors";
            select.buttons(i++).text.text = "usage";
            select.buttons(i++).text.text = "search";
            select.buttons(i++).text.text = "statistics";
            select.buttons(i++).text.text = "timing";
            select.maxwidth = max<int>();
            select.selected = 0;

            log_medias.view.wordwrap = false;
            log_audios.view.wordwrap = false;
            log_videos.view.wordwrap = false;
            log_search.view.wordwrap = false;
            log_medias.view.ellipsis = true;
            log_audios.view.ellipsis = true;
            log_videos.view.ellipsis = true;
            log_search.view.ellipsis = true;
            log_search.view.current_line_frame.color =
                rgba(150,150,150,64);

            if (app::appdata.error != "")
                log_medias << red(bold(
                app::appdata.error));
        }

        void reload ()
        {
            report::load();
            log_errors.clear(); log_errors << report::errors;
            log_usages.clear(); log_usages << report::usages;
            log_statts.clear(); log_statts << report::statts;
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
                int l = gui::metrics::line::width;
                int x = splitter.set("studio::dic::area::splitter", 25, 60, 75);

                selector.coord = xywh(0, 0, W, h);
                detail  .coord = xyxy(0, h, x, H);
                search  .coord = xyxy(0, h, x, H);

                for (int i=0; i<
                consoles.size(); i++)
                consoles[i]->coord = i <= last_narrow ?
                xyxy{x, h, W, H}:
                xyxy{0, h, W, H};
            }

            if (what == &selector)
            {
                int n = selector.object.selected.now;
                for (int i=0; i<
                consoles.size(); i++)
                consoles[i]->show(i == n);

                detail.show(n <= last_detail);
                search.show(n == inde_search);
            }

            if (what == &detail) {
                clicked = detail.object.clicked;
                notify();
            }

            for (auto console: consoles)
            if (what == &console->link)
            {
                str link = console->link;
                if (link.starts_with("file://"))
                    detail.object.select(
                        link.from(7));
                else
                if (auto index =
                app::vocabulary.index
                (link); index)
                {
                    clicked = *index;
                    notify();
                }
            }
        }
    };
}
