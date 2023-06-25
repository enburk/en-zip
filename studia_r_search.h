#pragma once
#include "app_one.h"
namespace studia::one
{
    struct search:
    widget<search>
    {
        using editor = gui::text::one_line_editor;
        using entry = content::out::course::search_entry;

        gui::text::view Starts;
        gui::text::view Medias;
        gui::area<editor> starts;
        gui::area<editor> medias;
        gui::area<gui::console> result;
        sys::thread  thread;
        array<entry> map;
        str link;

        search ()
        {
            Starts.text = "starts with:";
            Medias.text = "media contain:";
            result.object.view.wordwrap = false;
            result.object.view.ellipsis = true;
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
                Medias.coord = xywh(W-w, y, w, h); y += h;
                medias.coord = xywh(W-w, y, w, h); y += h;
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
                Medias.color = s.touched.first;
                edit(starts);
                edit(medias);
            }

            if (what == &result.object.link)
            {
                link = result.object.link;
                if (link.starts_with("clipboard://"))
                {
                    sys::clipboard::set(link.from(12));
                    return;
                }
                notify(&link);
            }

            if (what == &starts.object.update_text)
            {
                str s = starts.object.text; s.strip();
                if (s.size() < 2) return;

                medias.object.text = "";
                thread.stop = true;
                thread.join();
                result.object.
                    clear();

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

                    header = content::out::entry::pretty_link(header);

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
            if (what == &medias.object)
            {
                str s = medias.object.text; s.strip();
                if (s.size() < 2) return;

                starts.object.text = "";
                thread.stop = true;
                thread.join();
                result.object.
                    clear();

                if (app::vocabulary.size() == 0) return;
                auto i = app::vocabulary.lower_bound_case_insensitive(s);
                bool o = eng::equal_case_insensitive(
                     s,  app::vocabulary[i].title);

                medias.object.editor.color = o ?
                gui::skins[skin].dark.first :
                gui::skins[skin].error.first;
                if (not o) return;

                using index= media::index;
                array<index> audio;
                array<index> video;

                auto range =
                app::mediadata.entries_dic.equal_range(
                media::entry_index{i, 0}, [](auto a, auto b)
                { return a.entry < b.entry; });

                for (auto [entry, media]: range)
                {
                    auto& index = app::mediadata.media_index[media];

                    str s = doc::html::untagged(
                        media::canonical(index.title));

                    if (index.options.
                    contains("sound"))
                    s = "[" + s + "]";

                    str kind =
                        index.kind == "audio" ? green ("[audio]"):
                        index.kind == "video" ? purple("[video]"):
                        "";
                    str title =
                        index.kind == "audio" ? gray(s):
                        index.kind == "video" ? dark(s):
                        "";

                    str fn = str(
                    str2path(index.path).stem());
                    fn = un_msdos(fn);
                    fn.strip();

                    str yadda = fn.extract_from("###");
                    str meta  = fn.extract_from("{{");
                    str optio = fn.extract_from("##");
                    str links = fn.extract_from("[" );
                    str comnt = fn.extract_from("%%");
                    str sense = fn.extract_from("@" );
                    if (sense != "") fn += " @ " +
                        sense;

                    if (index.options.
                    contains("sound"))
                    fn += " # SOUND";

                    result.object << linked(
                    kind + " " + title,
                    "clipboard://: " + fn);
                }
            }
        }
    };
}
