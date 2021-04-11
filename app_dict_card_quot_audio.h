#pragma once
#include "app_dict_html.h"
#include "app_dict_media.h"
namespace app::dict::card
{
    struct audio : gui::widget<audio>
    {
        html_view text;
        sys::audio::player player;
        mediae::media_index index;
        gui::property<gui::time> timer;
        std::atomic<gui::media::state> state;
        gui::time start, stay;
        std::thread thread;
        str error;

        audio ()
        {
            on_change(&skin);
        }
        ~audio ()
        {
            if (thread.joinable())
                thread.join();
        }

        void reset (mediae::media_index index_)
        {
            start = gui::time{};
            stay  = gui::time{1000 +
                index_.title.size() * 90/1000 +
                index_.credit.size() * 10/1000 +
                index_.comment.size() * 30/1000
            };

            if (index == index_) return; else
                index =  index_;

            stop();
            
            state = gui::media::state::failure;

            if (index == mediae::media_index{}) return;

            state = gui::media::state::loading;

            if (thread.joinable())
                thread.join();

            thread = std::thread([this]()
            {
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
                    std::ifstream ifstream(path, std::ios::binary);

                    int size = length;
                    if (size == 0)
                    {
                        ifstream.seekg(0, std::ios::end);
                        size = (int)ifstream.tellg();
                    }
                    ifstream.seekg(offset, std::ios::beg);

                    data.resize(size);
                    ifstream.read((char*)(data.data()), size);

                    media::audio::decoder decoder(data);

                    player.load(
                        decoder.output,
                        decoder.channels,
                        decoder.samples,
                        decoder.bps
                    );

                    state = gui::media::state::ready;
                }
                catch (std::exception & e) {
                    state = gui::media::state::failure;
                    error = e.what();
                }
            });

            str c = index.credit;
            str s = index.title;

            s = ::app::dict::mediae::canonical(s);

            while (s.ends_with("<br>"))
                s.resize(s.size()-4);

            str date;
            for (str option : index.options)
                if (option.starts_with("date "))
                    date = option.from(5);

            if (date != "") c += ", <i>" + date + "</i>";

            if (index.comment != "") s += "<br><br>"
                "<font color=#808080><i>" + 
                ::app::dict::mediae::canonical(index.comment) +
                "</i></font>";

            s += "<br><br>"
                "<div style=\"margin-left: 3em\">"
                "<font color=#808080 size=80%>" + c +
                "</font></div>";

            s = eng::parser::embolden(s, text.excluded_links);

            text.it_is_a_title = true;
            text.html = s;
        }

        void play ()
        {
            switch(state.load()) {
            case gui::media::state::ready:
            case gui::media::state::playing:
            case gui::media::state::finished:
            {
                start = gui::time::now;
                state = gui::media::state::playing;
                auto duration = gui::time{(int)(player.duration*1000)};
                duration = max(duration, stay);
                player.play(0.0, 0.0);
                timer.go (gui::time{0}, gui::time{0});
                timer.go (gui::time{1}, duration);
                break;
            }
            default: break;
            }
        }

        void stop ()
        {
            switch(state.load()) {
            case gui::media::state::ready:
            case gui::media::state::playing:
            case gui::media::state::finished:

                player.stop(0.0);
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
            if (what == &text ) { clicked = text .clicked; notify(); }
        }
    };
}

