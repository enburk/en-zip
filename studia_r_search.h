#pragma once
#include "app_one.h"
namespace studia::one
{
    struct search:
    widget<search>
    {
        using editor = gui::text::one_line_editor;
        using entry = content::out::course::search_entry;

        gui::text::view Exact;
        gui::text::view EXact;
        gui::text::view Title;
        gui::text::view Words;
        gui::text::view Media;
        gui::area<editor> exact;
        gui::area<editor> eXact;
        gui::area<editor> title;
        gui::area<editor> words;
        gui::area<editor> media;
        gui::area<gui::console> result;
        sys::thread  thread;
        array<entry> titlemap;
        array<entry> wordsmap;
        str link;

        search ()
        {
            Exact.text = "exactly:";
            EXact.text = "has exactly:";
            Title.text = "starts with:";
            Words.text = "has word starting with:";
            Media.text = "media contain:";
            result.object.view.wordwrap = false;
            result.object.view.ellipsis = true;
            reload();
        }

        void reload (content::unit* unit = nullptr, str path = "")
        {
            exact.object.text = "";
            eXact.object.text = "";
            title.object.text = "";
            words.object.text = "";
            if (true) sys::in::file("../data/course_searchmap_title.dat") >> titlemap;
            if (true) sys::in::file("../data/course_searchmap_words.dat") >> wordsmap;
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
                Exact .coord = xywh(W-w, y, w, h); y += h;
                exact .coord = xywh(W-w, y, w, h); y += h;
                EXact .coord = xywh(W-w, y, w, h); y += h;
                eXact .coord = xywh(W-w, y, w, h); y += h;
                Title .coord = xywh(W-w, y, w, h); y += h;
                title .coord = xywh(W-w, y, w, h); y += h;
                Words .coord = xywh(W-w, y, w, h); y += h;
                words .coord = xywh(W-w, y, w, h); y += h;
                Media .coord = xywh(W-w, y, w, h); y += h;
                media .coord = xywh(W-w, y, w, h); y += h;
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

                Exact.color = s.touched.first;
                EXact.color = s.touched.first;
                Title.color = s.touched.first;
                Words.color = s.touched.first;
                Media.color = s.touched.first;
                edit(exact);
                edit(eXact);
                edit(title);
                edit(words);
                edit(media);
            }

            if (what == &result.object.link)
            {
                link = result.object.link;
                notify(&link);
            }

            if (what == &exact.object.update_text
            or  what == &eXact.object.update_text
            or  what == &title.object.update_text
            or  what == &words.object.update_text)
            {
                bool o0 = what == &exact.object.update_text;
                bool o1 = what == &eXact.object.update_text;
                bool o2 = what == &title.object.update_text;
                bool o3 = what == &words.object.update_text;

                auto& edit = o0 ? exact : o1 ? eXact : o2 ? title : words;
                auto& map = o0 or o2 ? titlemap : wordsmap;

                str s = edit.object.text; s.strip();
                s = eng::asciized(s).ascii_lowercased();

                if (s == "" or not o0
                and s.size() <= 1 and
                not content::search_symbols.contains(s))
                    return;

                if (not o0) exact.object.text = "";
                if (not o1) eXact.object.text = "";
                if (not o2) title.object.text = "";
                if (not o3) words.object.text = "";

                media.object.text = "";
                thread.stop = true;
                thread.join();
                result.object.
                    clear();

                auto lower = map.lower_bound(entry{s},
                    [](auto& a, auto &b) {
                    return a.word < b.word;
                    });

                auto upper = map.upper_bound(entry{s + "zzz"},
                    [](auto& a, auto &b) {
                    return a.word < b.word;
                    });

                array<entry> entries;
                for (auto it = lower; it != upper; ++it)
                if ((o0 or o1) and it->word == s or
                not (o0 or o1) and it->word.upto(s.size()) == s)
                    entries += *it;

                for (auto& e: entries)
                e.link.replace_all("| ", "|0");

                std::ranges::stable_sort(entries, {}, &entry::link);
                auto r = std::ranges::unique(entries, {}, &entry::link);
                entries.erase(r.begin(), r.end());

                for (auto& e: entries)
                e.link.replace_all("|0", "| ");

                str Header;

                for (auto& e: entries)
                {
                    str header = e.link;
                    int line = header.extract_from("|").stoi();
                    header = content::out::entry::pretty_link(header);

                    e.entry.replace_all("/", blue("/"));
                    e.entry.replace_all("\\", blue("\\"));
                    e.entry.replace_all("|", blue("|"));
                    e.entry.replace_all("@", blue("@"));
                    e.entry.replace_all("%", blue("%"));

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
            if (what == &media.object)
            {
                str src = media.object.text; src.strip();
                if (src.size() < 2 and
                not content::search_symbols.contains(src))
                    return;

                exact.object.text = "";
                eXact.object.text = "";
                title.object.text = "";
                words.object.text = "";
                thread.stop = true;
                thread.join();
                result.object.
                    clear();

                bool good = false;

                for (str word: eng::forms(src))
                {
                    if (app::vocabulary.size() == 0) return;
                    auto i = app::vocabulary.lower_bound_case_insensitive(word);

                    if (not
                        eng::equal_case_insensitive(word,
                        app::vocabulary[i].title))
                        continue;

                    good = true;

                    using index= media::index;
                    array<index> audio;
                    array<index> video;

                    auto range0 =
                    app::mediadata.entries_dic.equal_range(
                    media::entry_index{i, 0}, [](auto a, auto b)
                    { return a.entry < b.entry; });

                    auto range1 =
                    app::mediadata.entries_one.equal_range(
                    media::entry_index{i, 0}, [](auto a, auto b)
                    { return a.entry < b.entry; });

                    auto range = range0.empty() ? range1 : range0;

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
                            index.kind == "audio" ? dark(html(s)):
                            index.kind == "video" ? dark(html(s)):
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
                        if (sense != "") title += " @ " + sense;
                        if (sense != "") fn += " @ " + sense;

                        if (index.options.
                        contains("sound"))
                        fn += " # SOUND";

                        title.replace_all(word, bold(word));

                        fn.replace_all("_", "");

                        result.object << linked(
                        kind + " " + title,
                        "clipboard://: " + fn +
                        "file://" + index.path);
                    }
                }

                media.object.editor.color = good ?
                gui::skins[skin].dark.first :
                gui::skins[skin].error.first;
            }
        }
    };
}
