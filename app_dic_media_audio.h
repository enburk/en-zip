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
        media::media_index index;
        gui::property<gui::time> timer;
        std::atomic<gui::media::state> state;
        gui::time start, stay;
        bool muted = true;

        player ()
        {
            on_change(&skin);
            state = gui::media::state::finished;
        }

        void reset (media::media_index index_, array<str> links)
        {
            start = gui::time{};
            stay  = gui::time{1000 +
                index_.title.size() * 40 +
                index_.credit.size() * 10 +
                index_.comment.size() * 20
            };

            text.forbidden_links = links;

            if (index == index_) return; else
                index =  index_;

            stop();

            state = gui::media::state::loading;
        }

        void load (std::mutex& mutex, std::atomic<bool>& cancel)
        {

            media::media_index index;
            array<str> forbidden_links;
            {
                std::lock_guard lock{mutex};
                if (state != gui::media::state::loading)
                    return;

                index = this->index;
                forbidden_links = text.forbidden_links;
            }

            if (cancel) return;

            index.title = eng::parser::embolden(
                index.title, forbidden_links);

            str title = media::canonical(index.title);
            str credit = media::canonical(index.credit);
            str comment = media::canonical(index.comment);

            while (
                title.ends_with("<br>"))
                title.resize(
                title.size()-4);

            if (comment != "") title += "<br><br>" +
                gray(italic(comment));

            str date;
            for (str option : index.options)
                if (option.starts_with("date "))
                    date = option.from(5);
            if (date != "" and credit != "") credit += ", ";
            if (date != "") credit += italic(date);

            if (credit != "") {
                credit.replace_all(", read by", "<br>read by");
                credit.replace_all(", narrated by", "<br>narrated by");
                credit = "<div style=\"line-height: 20%\"><br></div>" +
                credit; }

            if (credit != "") title +="<br>"
                "<div style=\"margin-left: 1em\">"
                "<font size=\"90%\">" + light(credit) +
                "</font></div>";

            if (false) std::ofstream("test.quot.html") << title;
            if (false) std::ofstream("test.quot.html.txt")
                << doc::html::print(title);

            if (cancel) return;

            text.html = title;

            if (cancel) return;

            std::filesystem::path dir = "../data/app_dict";
            std::string storage = "storage." +
                std::to_string(index.location.source)
                    + ".dat";

            std::filesystem::path path = dir / storage;
            int offset = index.location.offset;
            int length = index.location.length;
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
                log << "audio failure: " +
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
                text.view.canvas.color = gui::skins[skin].light.first;
                text.alignment = XY{pix::left, pix::center};
                text.color = gui::skins[skin].touched.first;
            }
            if (what == &timer and timer == gui::time{1})
            {
                state = gui::media::state::finished;
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &text ) { clicked = text.clicked; notify(); }
        }
    };
}

