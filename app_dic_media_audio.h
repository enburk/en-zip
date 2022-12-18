#pragma once
#include "app_dic_html.h"
#include "app_dic_media.h"
namespace app::dic::audio
{
    struct player:
    widget<player>
    {
        html_view text;
        sys::audio::player audio;
        array<str> links, load_links;
        media::media_index index, load_index;
        std::atomic<gui::media::state> state;
        gui::property<gui::time> timer;
        gui::time start, stay;
        int clicked = 0;
        bool click = false;
        bool muted = true;
        str  html;

        player ()
        {
            on_change(&skin);
            state = gui::media::state::finished;
        }

        void reset (media::media_index index_, array<str> links_)
        {
            start = gui::time{};
            stay  = gui::time{1000 +
            index_.title.size() * 40 +
            index_.credit.size() * 10 +
            index_.comment.size() * 20};

            text.forbidden_links = links_;

            if (index == index_) return;

            index = load_index = index_;
            links = load_links = links_;

            stop();

            state = gui::media::state::loading;
        }

        void show (gui::time time = gui::time{})
        {
            if (html != "") {
            text.html = html; html = ""; }
            using base = widget<player>;
            base::show(time);

            logs::media << media::log(index);
        }

        void load (str entry_title, std::atomic<bool>& cancel)
        {
            if (state == gui::media::state::ready
            or  state == gui::media::state::playing
            or  state == gui::media::state::finished)
                return;

            str title = load_index.title;
            str credit = load_index.credit;
            str comment = load_index.comment;
            str sense;

            title.replace_all("---", mdash.data());
            title.replace_all("--" , ndash.data());

            if (title.ends_with("}")) {
                title.split_by("{",
                title, sense);
                title.strip();
                sense.strip();
                sense.truncate(); }

            if (array<str>{
                "1","2","3","+",
                "1,2","1,2,3"}.
                contains(sense))
                sense = "";
            
            title = eng::parser::embolden(
            title, load_links);

            title = media::canonical(title);
            credit = media::canonical(credit);
            comment = media::canonical(comment);

            if (load_index.options.contains("sound"))
                title = gray("[" + title + "]");

            if (sense != "") 
                title += "<br>""<small>" +
                gray(sense) + "</small>";

            while (
                title.ends_with("<br>"))
                title.resize(
                title.size()-4);

            if (comment != "") title += "<br><br>" +
                gray(italic(comment));

            str date;
            for (str option : index.options)
            if (option.starts_with("date ")) date = option.from(5);
            if (date != "" and credit != "") credit += ", ";
            if (date != "") credit += italic(date);

            if (credit != "") {
                credit.replace_all(", read by", "<br>read by");
                credit.replace_all(", narrated by", "<br>narrated by");
                credit = "<div style=\"line-height: 20%\"><br></div>" +
                credit; }

            if (credit != "") title +="<br>"
            //  "<div style=\"margin-left: 1em\">"
                "<font size=\"90%\">" + light(credit) +
                "</font>"
            //  "</div>"
                ;

            if (false) std::ofstream("test.quot.html") << title;
            if (false) std::ofstream("test.quot.html.txt")
                << doc::html::print(title);

            html = title;

            if (cancel) return;

            std::filesystem::path dir = "../data/app_dict";
            std::string storage = "storage." +
                std::to_string(load_index.location.source)
                    + ".dat";

            std::filesystem::path path = dir / storage;
            int offset = load_index.location.offset;
            int length = load_index.location.length;
            array<sys::byte> data;

            try
            {
                data.resize(length);
                std::ifstream ifstream(path, std::ios::binary);
                ifstream.seekg(offset, std::ios::beg);
                ifstream.read((char*)(data.data()), length);

                if (cancel) return;

                ::media::audio::decoder decoder(data);

                if (cancel) return;

                audio.load(
                    decoder.output,
                    decoder.channels,
                    decoder.samples,
                    decoder.bps
                );

                if (cancel) return;

                state = gui::media::state::ready;
            }
            catch (std::exception const& e) {
                state = gui::media::state::failure;
                logs::times << "audio failure: " +
                    str(e.what());
            }
        }

        void play ()
        {
            switch(state) {
            case gui::media::state::ready:
            case gui::media::state::finished:
            {
                start = gui::time::now;
                state = gui::media::state::playing;

                if (muted) {
                    timer.go (gui::time{0}, gui::time{0});
                    timer.go (gui::time{1}, gui::time{3*stay.ms});
                    break; }

                auto duration = gui::time{(int)(audio.duration*1000)};
                duration = max(duration, stay);
                audio.play(0.0, 0.0);
                timer.go (gui::time{0}, gui::time{0});
                timer.go (gui::time{1}, duration);
                break;
            }
            default: break;
            }
        }

        void stop ()
        {
            switch(state) {
            case gui::media::state::ready:
            case gui::media::state::playing:

                audio.stop(0.0);
                state = gui::media::state::finished;
                timer.go (gui::time{},
                          gui::time{});
                break;

            default: break;
            }
        }

        void mute (bool mute)
        {
            if (state == gui::media::state::playing)
            {
                if (mute)
                {
                    audio.stop(0.0);
                }
                else
                {
                    start = gui::time::now;
                    auto duration = gui::time{(int)(audio.duration*1000)};
                    duration = max(duration, stay);
                    audio.play(0.0, 0.0);
                    timer.go (gui::time{0}, gui::time{0});
                    timer.go (gui::time{1}, duration);
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
                text.canvas.color = gui::skins[skin].light.first;
                text.alignment = xy{pix::left, pix::center};
                text.color = gui::skins[skin].touched.first;
            }
            if (what == &timer)
            {
                if (timer == gui::time{1})
                {
                    if (text.link != "") {
                        timer.go (gui::time{0}, gui::time{0});
                        timer.go (gui::time{1}, 1s);
                        return; }

                    state = gui::media::state::finished;
                }
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

