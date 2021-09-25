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

            str s = index.title;
            str c = index.credit;
            str i = index.comment;

            str title = media::canonical(index.title);
            s = media::canonical(s);
            c = media::canonical(c);
            i = media::canonical(i);

            while (s.ends_with("<br>"))
                s.resize(s.size()-4);

            str date;
            for (str option : index.options)
                if (option.starts_with("date "))
                    date = option.from(5);

            if (date != "") c += ", <i>" + date + "</i>";

            if (i != "") s += "<br><br>"
                "<font color=#808080><i>" + i +
                "</i></font>";

            s +="<br>"
                "<div style=\"margin-left: 3em\">"
                "<font color=#A0A0A0 size=\"90%\">" + c +
                "</font></div>";

            //s = eng::parser::embolden(s, text.excluded_links);

            if (false) std::ofstream("test.quot.html") << s;
            if (false) std::ofstream("test.quot.html.txt")
                << doc::html::print(s);

            text.html = s;
        }

        void load (std::mutex& mutex, std::atomic<bool>& cancel)
        {
            if (cancel) return;

            media::media_index index;
            {
                std::lock_guard lock{mutex};
                if (state != gui::media::state::loading)
                    return;

                index = this->index;
            }

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
            case gui::media::state::playing:
            case gui::media::state::finished:
            {
                start = gui::time::now;
                state = gui::media::state::playing;
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

