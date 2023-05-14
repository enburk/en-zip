#pragma once
#include "app_one.h"
namespace studio::one
{
    struct search:
    widget<search>
    {
        using editor = gui::text::one_line_editor;
        using entry = content::out::course::search_entry;

        gui::text::view Starts;
        gui::area<editor> starts;
        gui::area<gui::console> result;
        sys::thread  thread;
        array<entry> map;
        str link;

        search ()
        {
            Starts.text = "starts with:";
            reload();
        }

        void reload (content::unit* unit = nullptr, str path = "")
        {
            starts.object.text = "";
            if (true) sys::in::file(
            "../data/course_searchmap.dat")
            >> map;
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*10;
                int h = gui::metrics::text::height*13/10;
                int x = 0;
                int y = 0;

                result.coord = xywh(0, 0, W-w, H);
                Starts.coord = xywh(W-w, y, w, h); y += h;
                starts.coord = xywh(W-w, y, w, h); y += h;
            }
            if (what == &skin)
            {
                auto s = gui::skins[skin];
                auto l = gui::metrics::line::width;
                auto edit = [s,l](auto& ed) {
                    ed.object.canvas.color = s.ultralight.first;
                    ed.object.alignment = xy{pix::center, pix::center};
                    ed.object.padding = xyxy{2*l, 2*l, 2*l, 2*l};
                    ed.show_focus = true; };

                Starts.color = s.touched.first;
                edit(starts);
            }

            if (what == &result.object.link)
            {
                link = result.object.link;
                notify();
            }

            if (what == &starts.object.update_text)
            {
                thread.stop = true;
                thread.join();
                result.object.
                    clear();

                str s = starts.object.text;
                if (s.size() < 2) return;

                auto range = map.equal_range(
                entry{s}, [](auto& a, auto &b)
                {
                    str s = b.word.
                    upto(a.word.size());
                    s = eng::asciized(s).
                    ascii_lowercased();
                    return a.word < s;
                });

                array<entry> entries;
                for (auto e: range)
                    entries += e;

                std::ranges::stable_sort(
                    entries, {}, &
                    entry::link);

                str Header;

                for (auto& e: entries)
                {
                    str header = e.link;
                    int line = std::stoi(
                        header.extract_from("|"));

                    header.replace_all("\\", "/");
                    header.resize(header.size()-4); // .txt
                    array<str> ss = header.split_by("/");
                    ss.upto(1).erase(); // content/
                    for (str& s: ss) {
                    s = s.from(3);
                    if (s.starts_with("''")
                    and s.  ends_with("''")) {
                        s.truncate(); s.erase(0);
                        s.truncate(); s.erase(0);
                        s = extracolor(
                        s); }
                    }
                    header = str(ss, blue("/"));

                    if (Header != header)
                    if (Header != "")
                        result.object << "<br>";

                    if (Header != header)
                        Header  = header,
                        result.object << dark(
                        header);

                    result.object << linked(
                    blue(monospace(std::format(
                    "{:2}: ", line+1))) +
                    e.entry,
                    e.link);
                }
            }
        }
    };
}
