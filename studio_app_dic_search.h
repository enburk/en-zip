#pragma once
#include "app_dic.h"
namespace studio::dic
{
    struct search:
    widget<search>
    {
        sys::thread thread;

        gui::text::view Voc_contains;
        gui::text::view Voc_endswith;
        gui::text::view Dic_contains;
        gui::area<gui::text::one_line_editor> voc_contains;
        gui::area<gui::text::one_line_editor> voc_endswith;
        gui::area<gui::text::one_line_editor> dic_contains;
        gui::unary_property<str> request;
        gui::button lexical_sense;
        gui::button lexical_brief;
        gui::button lexical_notes;
        gui::button related_items;
        gui::console loading;
        gui::console counter;
        gui::console* result;

        search ()
        {
            Voc_contains.text = "vocabulary contains:";
            Voc_endswith.text = "vocabulary ends with:";
            Dic_contains.text = "dictionary contains:";
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

        void out (str title, int n)
        {
            str
            number = std::to_string(n);
            number.align_right(5);
            number = gray(monospace(
            number));

            title = doc::html::encoded(title);

            *result << "<a href=\""
            + title + "\">"
            + number + ". "
            + title + "</a>";
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

                Voc_contains.coord = xywh(0, y, W, h); y += h;
                voc_contains.coord = xywh(0, y, W, h); y += h;
                Voc_endswith.coord = xywh(0, y, W, h); y += h;
                voc_endswith.coord = xywh(0, y, W, h); y += h;
                Dic_contains.coord = xywh(0, y, W, h); y += h;
                dic_contains.coord = xywh(0, y, W, h); y += h;

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
                Voc_contains.color = s.touched.first;
                Voc_endswith.color = s.touched.first;
                Dic_contains.color = s.touched.first;
                voc_contains.object.canvas.color = s.ultralight.first;
                voc_endswith.object.canvas.color = s.ultralight.first;
                dic_contains.object.canvas.color = s.ultralight.first;
                voc_contains.object.alignment = xy{pix::center, pix::center};
                voc_endswith.object.alignment = xy{pix::center, pix::center};
                dic_contains.object.alignment = xy{pix::center, pix::center};
                voc_contains.object.padding = xyxy{2*l, 2*l, 2*l, 2*l};
                voc_endswith.object.padding = xyxy{2*l, 2*l, 2*l, 2*l};
                dic_contains.object.padding = xyxy{2*l, 2*l, 2*l, 2*l};
                voc_contains.show_focus = true;
                voc_endswith.show_focus = true;
                dic_contains.show_focus = true;
            }

            if (what == &voc_contains)
            {
                thread.stop = true;
                thread.join();
                result->clear();

                int n = 0;
                str s = voc_contains.object.text;
                if (s != "")
                for(auto& entry: app::dic::vocabulary.data)
                if (entry.title.contains(s))
                out(entry.title, ++n);
            }
            if (what == &voc_endswith)
            {
                thread.stop = true;
                thread.join();
                result->clear();

                int n = 0;
                str s = voc_endswith.object.text;
                if (s != "")
                for(auto& entry: app::dic::vocabulary.data)
                if (entry.title.ends_with(s))
                out(entry.title, ++n);
            }

            if (what == &voc_contains) request = "";
            if (what == &voc_endswith) request = "";
            if (what == &dic_contains) request = dic_contains.object.text;

            if (what == &request
            or  what == &lexical_sense
            or  what == &lexical_brief
            or  what == &lexical_notes
            or  what == &related_items)
            {
                str s = request.now;
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
                        goto not_found;
                        found: out(entry.title, ++m);
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
