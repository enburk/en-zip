#pragma once
#include "app_dic.h"
namespace studio::dic
{
    struct search:
    widget<search>
    {
        sys::thread thread;

        gui::area<
        gui::text::one_line_editor> Request;
        gui::text::one_line_editor& request = Request.object;
        gui::console* result;
        gui::console loading;
        gui::console counter;
        gui::button lexical_sense;
        gui::button lexical_brief;
        gui::button lexical_notes;
        gui::button related_items;

        search ()
        {
            lexical_sense.text.text = "senses";
            lexical_brief.text.text = "examples";
            lexical_notes.text.text = "notes";
            related_items.text.text = "related";
            lexical_sense.kind = gui::button::toggle;
            lexical_brief.kind = gui::button::toggle;
            lexical_notes.kind = gui::button::toggle;
            related_items.kind = gui::button::toggle;
            lexical_sense.on = true;
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
                int h = gui::metrics::text::height*13/10;
                int p = H/100;
                int x = 0;
                int y = 0;

                Request.coord = xywh(0, y, W, h); y += h;

                lexical_sense.coord = xywh(x, y, W/4, h); x += W/4;
                lexical_brief.coord = xywh(x, y, W/4, h); x += W/4;
                lexical_notes.coord = xywh(x, y, W/4, h); x += W/4;
                related_items.coord = xywh(x, y, W/4, h); y += h;

                loading.coord = xywh(0, y, W, h); y += h;
                counter.coord = xywh(0, y, W, h); y += h;

            }
            if (what == &skin)
            {
                auto s = gui::skins[skin];
                auto l = gui::metrics::line::width;

                loading.canvas.color = rgba{};
                counter.canvas.color = rgba{};
                request.canvas.color = s.ultralight.first;
                request.padding = xyxy{2*l, 2*l, 2*l, 2*l};
                Request.show_focus = true;
            }
            if (what == &request
            or  what == &lexical_sense
            or  what == &lexical_brief
            or  what == &lexical_notes
            or  what == &related_items)
            {
                str s = request.text;
                if (s == "") return;

                thread = [this, s](auto& cancel)
                {
                    bool lexical_sense = this->lexical_sense.on.now;
                    bool lexical_brief = this->lexical_brief.on.now;
                    bool lexical_notes = this->lexical_notes.on.now;
                    bool related_items = this->related_items.on.now;

                    load_dictionary(loading);

                    result->clear();

                    int n = app::dic::vocabulary.size();
                    int i = 0;
                    int m = 0;

                    for (auto& entry: dictionary.entries)
                    {
                        if (cancel) return;

                        for (auto& topic: entry.topics)
                        for (auto& sense: topic.content)
                        {
                            if (not lexical_sense
                            and not lexical_brief and
                               eng::lexical_items.contains(topic.header))
                               continue;

                            if (not lexical_notes and
                               eng::lexical_notes.contains(topic.header))
                               continue;

                            if (not related_items and
                               eng::related_items.contains(topic.header))
                               continue;

                            if (not lexical_sense
                            and not sense.starts_with("#:")
                            and not sense.starts_with("##:") and
                               eng::lexical_items.contains(topic.header))
                               continue;

                            if (not lexical_brief
                            and(sense.starts_with("#:")
                            or  sense.starts_with("##:")) and
                               eng::lexical_items.contains(topic.header))
                               continue;

                            if (sense.contains(s))
                                goto found;
                        }
                        goto not_found; found:
                        {
                            m++;
                            str title = 
                            doc::html::encoded(entry.title);
                            str number = std::to_string(m);
                            number.align_right(5);;
                            *result << "<a href=\""
                            + title + "\">"
                            + gray(monospace(number)) + ". "
                            + title + "</a>";
                        }
                        not_found:

                        i++;
                        if (i == n
                        or  i % 1000 == 0) {
                            counter.clear();
                            counter << gray("scan: " 
                            + std::to_string(i) + " of "
                            + std::to_string(n) + " entries ("
                            + std::to_string(m) + " matches)"); }

                        if (m == 10000)
                            break;
                    }
                };
            }
        }
    };
}
