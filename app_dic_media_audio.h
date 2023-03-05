#pragma once
#include "eng_phenomena.h"
#include "app_dic_html.h"
#include "app_dic_media.h"
namespace app::dic::audio
{
    struct player:
    widget<player>
    {
        typedef gui::time time;
        typedef sfx::media::state state;

        html_view text;
        sfx::media::audio::player audio;
        array<str> links, load_links;
        media::media_index index, load_index;
        std::atomic<state> status;
        gui::property<time> timer;
        time start, stay;
        int  clicked = 0;
        bool click = false;
        bool muted = true;
        str  html;

        player ()
        {
            on_change(&skin);
            status = state::finished;
        }

        void reset (media::media_index index_, array<str> links_)
        {
            start = time{};
            stay  = time{1000 +
            index_.title.size() * 40 +
            index_.credit.size() * 10 +
            index_.comment.size() * 20};

            text.forbidden_links = links_;

            if (index == index_) return;

            index = load_index = index_;
            links = load_links = links_;

            stop();

            status = state::loading;
        }

        void show (time time = time{})
        {
            if (html != "") {
            text.html = html; html = ""; }
            using base = widget<player>;
            base::show(time);

            logs::media << media::log(index);
        }

        void load (str entry_title, std::atomic<bool>& cancel)
        {
            if (status == state::ready
            or  status == state::playing
            or  status == state::finished)
                return;

            std::filesystem::path dir = "../data/media";
            std::string storage = "storage." +
                std::to_string(load_index.location.source)
                    + ".dat";

            audio.load(
            sys::in::bytes(dir/storage,
            load_index.location.offset,
            load_index.location.length));

            str title   = load_index.title;
            str sense   = load_index.sense;
            str credit  = load_index.credit;
            str comment = load_index.comment;

            title.replace_all("---", mdash);
            title.replace_all("--" , ndash);

            title = eng::parser::embolden(title, load_links);

            title   = media::canonical(title);
            credit  = media::canonical(credit);
            comment = media::canonical(comment);

            if (load_index.options.contains("sound"))
            title = dark("[" + title + "]");

            while (
            title.ends_with("<br>"))
            title.resize(
            title.size()-4);

            if (comment == "" and sense != "")
            if (eng::lexical_items  .contains(sense)
            or  eng::lexical_notes  .contains(sense)
            or  eng::related_items  .contains(sense)
            or  eng::list::sensitive.contains(sense))
                comment = sense;

            if (comment != "")
            title += "<br>" +
            gray(small(comment));

            str date;
            for (str option : index.options)
            if (option.starts_with("date ")) date = option.from(5);
            if (date != "" and credit != "") credit += ", ";
            if (date != "") credit += italic(date);

            if (credit != "") {
                credit.replace_all(", read by", "<br>read by");
                credit.replace_all(", narrated by", "<br>narrated by"); }

            if (credit != "")
            title += "<br><div style=\"line-height: 20%\"><br></div>" +
            light(small(credit));

            if (false) std::ofstream("test.quot.html") << title;
            if (false) std::ofstream("test.quot.html.txt")
                << doc::html::print(title);

            html = title;

            status = state::ready;
        }

        void play ()
        {
            switch(status) {
            case state::ready:
            case state::finished:
            {
                start = time::now;
                status = state::playing;

                if (muted) {
                    timer.go (time{0}, time{0});
                    timer.go (time{1}, time{3*stay.ms});
                    break; }

                auto duration = audio.duration;
                duration = max(duration, stay);
                audio.play();
                timer.go (time{0}, time{0});
                timer.go (time{1}, duration);
                break;
            }
            default: break;
            }
        }

        void stop ()
        {
            switch(status) {
            case state::ready:
            case state::playing:

                audio.stop();
                status = state::finished;
                timer.go (time{},
                          time{});
                break;

            default: break;
            }
        }

        void mute (bool mute)
        {
            if (status == state::playing)
            {
                if (mute)
                {
                    audio.stop();
                }
                else
                {
                    start = time::now;
                    auto duration = audio.duration;
                    duration = max(duration, stay);
                    audio.play();
                    timer.go (time{0}, time{0});
                    timer.go (time{1}, duration);
                }
            }
            muted = mute;
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                text.coord = coord.now.local();
            }
            if (what == &skin)
            {
                text.alignment = xy{pix::left, pix::center};
                text.color = gui::skins[skin].touched.first;
            }
            if (what == &timer and timer == time{1})
            {
                if (text.link != "") {
                timer.go(time{0}, 0s);
                timer.go(time{1}, 1s); }
                else status = state::finished;
            }
            if (what == &text )
            {
                clicked = text.clicked;
                click = true; notify();
                click = false;
            }
        }
    };
}

