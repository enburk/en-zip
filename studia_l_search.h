#pragma once
#include "app_dic.h"
#include "studio_dic.h"
namespace studia::dic
{
    struct search:
    widget<search>
    {
        sys::thread thread;

        struct wordlisttype
        {
            array<str> words;
            wordlisttype() = default;
            wordlisttype(str filename)
            {
                words.clear();
                str text = sys::
                optional_text(str2path(filename));
                for (str line: text.lines())
                {
                    line.strip(); if (line == "") continue;
                    for (str word: line.split_strip_by("|"))
                    if (word != "") words += word;
                }
            }
        };
        wordlisttype wordlist;

        gui::text::view Voc_contains;
        gui::text::view Voc_endswith;
        gui::text::view Dic_contains;
        gui::text::view Wordlistfile;
        gui::area<gui::text::one_line_editor> voc_contains;
        gui::area<gui::text::one_line_editor> voc_endswith;
        gui::area<gui::text::one_line_editor> dic_contains;
        gui::area<gui::text::one_line_editor> wordlistfile;
        gui::unary_property<str> request;
        gui::button lexical_sense;
        gui::button lexical_brief;
        gui::button lexical_notes;
        gui::button related_items;
        gui::button recursive;
        gui::button synonyms;
        gui::button antonyms;
        gui::button hypernyms;
        gui::button hyponyms;
        gui::button meronyms;
        gui::button holonyms;
        gui::button troponyms;

        gui::text::view Irrenouns;
        gui::text::view Irreverbs;
        gui::widgetarium<gui::button> irrenouns;
        gui::widgetarium<gui::button> irreverbs;

        gui::console loading;
        gui::console counter;
        gui::console result;
        gui::splitter splitter;
        str& link = result.link;

        search ()
        {
            Voc_contains.text = "vocabulary contains:";
            Voc_endswith.text = "vocabulary ends with:";
            Dic_contains.text = "dictionary contains:";
            Wordlistfile.text = "word list file:";
            auto button = [](gui::button& button, str text, bool on=false) {
                button.kind = gui::button::toggle;
                button.text.text = text;
                button.on = on;
            };
            button(lexical_sense, "senses", true);
            button(lexical_brief, "examples");
            button(lexical_notes, "notes"   );
            button(related_items, "related" );
            button(recursive, "occurences");
            button(synonyms,  "synonyms" , true);
            button(antonyms,  "antonyms" , true);
            button(hypernyms, "hypernyms", true);
            button(hyponyms,  "hyponyms" , true);
            button(meronyms,  "meronyms" , true);
            button(holonyms,  "holonyms" , true);
            button(troponyms, "troponyms", true);

            int n = 0;
            int v = 0;
            Irrenouns.text = "irregular nouns:";
            Irreverbs.text = "irregular nouns:";
        }

        void out (str title, int n)
        {
            str
            number = std::to_string(n);
            number.align_right(5);
            number = gray(monospace(
            number));

            result << linked(
            number + ". " +
            html(title),
            title);
        }

        void out (str text, str title = "")
        {
            result << linked(
            gray(text) +
            html(title),
            title);
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
                int s = splitter.set("studia::dic::search::splitter", 25, 60, 75);
                int p = H/100;
                int x = 0;
                int y = 0;

                Voc_contains.coord = xywh(0, y, s, h); y += h;
                voc_contains.coord = xywh(0, y, s, h); y += h;
                Voc_endswith.coord = xywh(0, y, s, h); y += h;
                voc_endswith.coord = xywh(0, y, s, h); y += h;
                Dic_contains.coord = xywh(0, y, s, h); y += h;
                dic_contains.coord = xywh(0, y, s, h); y += h;

                lexical_sense.coord = xywh(x, y, s/4, h); x += s/4;
                lexical_brief.coord = xywh(x, y, s/4, h); x += s/4;
                lexical_notes.coord = xywh(x, y, s/4, h); x += s/4;
                related_items.coord = xywh(x, y, s/4, h); y += h; x = 0;

                Wordlistfile.coord = xywh(0, y, s, h); y += h;
                wordlistfile.coord = xywh(0, y, s, h); y += h;

                recursive.coord = xywh(x, y, s/4, h); x += s/4;
                synonyms .coord = xywh(x, y, s/4, h); x += s/4;
                antonyms .coord = xywh(x, y, s/4, h); x += s/4;
                hypernyms.coord = xywh(x, y, s/4, h); y += h; x = 0;

                hyponyms .coord = xywh(x, y, s/4, h); x += s/4;
                meronyms .coord = xywh(x, y, s/4, h); x += s/4;
                holonyms .coord = xywh(x, y, s/4, h); x += s/4;
                troponyms.coord = xywh(x, y, s/4, h); y += h; x = 0;

                loading.coord = xywh(0, y, s, h); y += h;
                counter.coord = xywh(0, y, s, h); y += h;

                result.coord = xyxy(s, 0, W, H);
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

                loading.canvas.color = rgba{};
                counter.canvas.color = rgba{};
                Voc_contains.color = s.touched.first;
                Voc_endswith.color = s.touched.first;
                Dic_contains.color = s.touched.first;
                Wordlistfile.color = s.touched.first;
                edit(voc_contains);
                edit(voc_endswith);
                edit(dic_contains);
                edit(wordlistfile);
            }

            if (what == &voc_contains)
            {
                thread.stop = true;
                thread.join();
                result.clear();

                int n = 0;
                str s = voc_contains.object.text;
                if (s != "")
                for(auto& entry: app::vocabulary.data)
                if (entry.title.contains(s))
                out(entry.title, ++n);
            }
            if (what == &voc_endswith)
            {
                thread.stop = true;
                thread.join();
                result.clear();

                int n = 0;
                str s = voc_endswith.object.text;
                if (s != "")
                for(auto& entry: app::vocabulary.data)
                if (entry.title.ends_with(s))
                out(entry.title, ++n);
            }

            if (what == &voc_contains) request = "";
            if (what == &voc_endswith) request = "";
            if (what == &dic_contains) request = dic_contains.object.text;
            if (what == &wordlistfile) request = dic_contains.object.text;
            if (what == &wordlistfile) wordlist = wordlisttype(wordlistfile.object.text);
            if (what == &wordlistfile) wordlistfile.object.color = wordlist.words.size() > 0 ?
                gui::skins[skin].heavy.first:
                gui::skins[skin].error.first;

            if (what == &request
            or  what == &lexical_sense
            or  what == &lexical_brief
            or  what == &lexical_notes
            or  what == &related_items
            or  what == &wordlistfile
            or  what == &recursive
            or  what == &synonyms
            or  what == &antonyms
            or  what == &hypernyms
            or  what == &hyponyms
            or  what == &meronyms
            or  what == &holonyms
            or  what == &troponyms)
            {
                str s = request.now;
                if (s == "" and wordlist.words.empty())
                    return;

                thread = [this, s](auto& cancel)
                {
                    studio::load_dictionary(loading);

                    result.clear();

                    if (s != "")
                    {
                        bool lexical_sense = this->lexical_sense.on.now;
                        bool lexical_brief = this->lexical_brief.on.now;
                        bool lexical_notes = this->lexical_notes.on.now;
                        bool related_items = this->related_items.on.now;

                        int n = app::vocabulary.size();
                        int i = 0;
                        int m = 0;

                        for (auto& entry: studio::dictionary.entries)
                        {
                            if (cancel) return;

                            for (auto& topic: entry.topics)
                            for (auto& sense: topic.content)
                            {
                                if (not lexical_sense
                                and not lexical_brief and
                                   eng::lexical_items.contains(
                                   topic.header))
                                   continue;

                                if (not lexical_notes and
                                   eng::lexical_notes.contains(
                                   topic.header))
                                   continue;

                                if (not related_items and
                                   eng::related_items.contains(
                                   topic.header))
                                   continue;

                                if (not lexical_sense
                                and not sense.starts_with("#:")
                                and not sense.starts_with("##:") and
                                   eng::lexical_items.contains(
                                   topic.header))
                                   continue;

                                if (not lexical_brief
                                and(sense.starts_with("#:")
                                or  sense.starts_with("##:")) and
                                   eng::lexical_items.contains(
                                   topic.header))
                                   continue;

                                if (sense.starts_with("# (''archaic")
                                or  sense.starts_with("## (''archaic")
                                or  sense.starts_with("# (''obsolete")
                                or  sense.starts_with("## (''obsolete")
                                or  sense.starts_with("# (''now rare")
                                or  sense.starts_with("## (''now rare"))
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
                    }
                    else if (this->recursive.on.now)
                    {
                        array<str> words = wordlist.words;
                        std::ranges::sort(words);

                        std::map<str, array<str>> matchmap;

                        int n = app::vocabulary.size();
                        int i = 0;
                        int m = 0;

                        for (auto& entry: studio::dictionary.entries)
                        {
                            if (cancel) return;

                            for (auto& topic: entry.topics) if (eng::lexical_items.contains(topic.header))
                            for (auto& sense: topic.content)
                            {
                                if (not sense.starts_with("# ")
                                and not sense.starts_with("## "))
                                    continue;

                                if (sense.starts_with("# (''archaic")
                                or  sense.starts_with("## (''archaic")
                                or  sense.starts_with("# (''obsolete")
                                or  sense.starts_with("## (''obsolete")
                                or  sense.starts_with("# (''now rare")
                                or  sense.starts_with("## (''now rare"))
                                    continue;

                                str s = sense;
                                for (auto& c: s)
                                if((c < 'A') || ('Z' < c &&
                                    c < 'a') || ('z' < c))
                                    c = ' ';
                                s.canonicalize();
                                s = s.ascii_lowercased();
                                array<str> ss = s.split_by(' ');
                                ss.deduplicate();
                                std::ranges::sort(ss);
                                array<str> matches;
                                std::ranges::set_intersection(words, ss,
                                std::back_inserter(matches));
                                for (auto match: matches)
                                {
                                    if (matchmap[match].size() == 0
                                    or  matchmap[match].back() != entry.title)
                                        matchmap[match] += entry.title;
                                    if (matchmap[match].size() >= 100)
                                        words.try_erase(match);
                                }
                            }

                            i++;
                            if (i == n
                            or  i % 1000 == 0) {
                                counter.clear();
                                counter << gray("scan: " 
                                + std::to_string(i) + " of "
                                + std::to_string(n) + " entries"); }
                        }

                        std::map<str, int> all;

                        for (auto& [word, matches]: matchmap)
                        {
                            bool first = true;
                            if (matches.size() == 0) continue;
                            if (matches.size() >= 100) out("TOO MATCH: " + word); else
                            for (str s: matches)
                            {
                                bool found = false;
                                for (str word: wordlist.words)
                                if (s.starts_with(word)) {
                                found = true; break; }
                                if (found) continue;

                                all[s]++;
                                if (all[s] > 1) continue;
                                if (first) out("FOUND FOR: ", word);
                                if (first) first = false;
                                out(s, ++m);
                            }
                        }

                        m = 0; out(""); out("1:"); for (auto [s, n]: all) if (n == 1) out(s, ++m);
                        m = 0; out(""); out("2:"); for (auto [s, n]: all) if (n == 2) out(s, ++m);
                        m = 0; out(""); out("3:"); for (auto [s, n]: all) if (n >= 3) out(s, ++m);
                    }
                    else
                    {
                        int n = wordlist.words.size();
                        int i = 0;
                        int m = 0;

                        bool synonyms  = this->synonyms .on.now;
                        bool antonyms  = this->antonyms .on.now;
                        bool hypernyms = this->hypernyms.on.now;
                        bool hyponyms  = this->hyponyms .on.now;
                        bool meronyms  = this->meronyms .on.now;
                        bool holonyms  = this->holonyms .on.now;
                        bool troponyms = this->troponyms.on.now;

                        std::set<str> all;

                        for (auto word: wordlist.words)
                        {
                            if (cancel) return;

                            {
                                bool first = true;

                                auto index = app::vocabulary.index(word);
                                if (!index) { out("NOT FOUND: " + word);
                                    continue; }

                                auto& entry = studio::dictionary.entries[*index];

                                for (auto& topic: entry.topics)
                                {
                                    if (topic.header == "synonyms"  and synonyms 
                                    or  topic.header == "antonyms"  and antonyms 
                                    or  topic.header == "hypernyms" and hypernyms
                                    or  topic.header == "hyponyms"  and hyponyms 
                                    or  topic.header == "meronyms"  and meronyms 
                                    or  topic.header == "holonyms"  and holonyms 
                                    or  topic.header == "troponyms" and troponyms)
                                    for (auto& text: topic.content)
                                    {
                                        for (str s: text.split_by(","))
                                        {
                                            s.strip();
                                            if (not wordlist.words.contains(s)
                                            and app::vocabulary.index(s))
                                            {
                                                if (all.contains(s)) continue; all.insert(s);
                                                if (first) out("FOUND FOR: ", word);
                                                if (first) first = false;
                                                out(s, ++m);
                                            }
                                        }
                                    }
                                }
                            }

                            i++;
                            if (i == n
                            or  i % 1 == 0) {
                                counter.clear();
                                counter << gray("scan: " 
                                + std::to_string(i) + " of "
                                + std::to_string(n) + " entries ("
                                + std::to_string(m) + " matches)"); }

                            if (m == 10000)
                                break;
                        }

                        m = 0;
                        out("");
                        out("IN ALPHABETICAL ORDER:");
                        for (str s: all) out(s, ++m);
                    }
                };
            }

            if (what == &link)
                notify (&link),
                notify (&request);

            if (what == &link)
            {
                array<gui::text::range> highlights;
                auto& view = result.view;
                auto& text = view.model.now->block;
                for (auto& line : text.lines)
                {
                    str s;
                    for (auto& token : line.tokens)
                    s += token.text;

                    if (not s.contains(link))
                        continue;

                    for (auto& token : line.tokens)
                    highlights += token.range;
                    view.highlights =
                    highlights;
                    break;
                }
            }

        }
    };
}
