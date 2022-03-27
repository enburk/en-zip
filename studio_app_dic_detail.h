#pragma once
#include "app_dic.h"
namespace studio::dic
{
    std::map<int, std::map<int, str>> locations;

    struct detail:
    widget<detail>
    {
        gui::text::view   path;
        gui::text::editor text;

        expected<nothing> select (str info)
        {
            str source_;
            str offset_;
            info.split_by(":", source_, offset_);
            int source = std::stoi(source_);
            int offset = std::stoi(offset_);

            auto i = locations.find(source);
            if (i == locations.end()) return
                aux::error("locations: not found source "
                    + source_);

            auto j = i->second.find(offset);
            if (j == i->second.end()) return
                aux::error("locations: not found offset "
                    + offset_);

            path.text = j->second;
            return nothing{};
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*7;
                int h = gui::metrics::text::height*12/7;

                path.coord = xyxy(0, 0, W, H/2);
                text.coord = xyxy(0, H/2, W, H);
            }
        }
    };
}
