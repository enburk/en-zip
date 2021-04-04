#pragma once
#include "app_dict_html.h"
#include "app_dict_media.h"
namespace app::dict::card
{
    struct quot : gui::widget<quot>
    {
        html_view text;
        sys::audio::player player;

        void reset_audio ()
        {
            auto indices = mediae::selected_audio;

            if (indices.empty()) return;

            auto index = indices[0];

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

                player.play(0.0, 0.0);
            }
            catch (std::exception & e) {
                //error = e.what();
                //state = media::state::failure;
            }

            str c = index.credit;
            str s = index.title;

            s = ::app::dict::mediae::canonical(s);

            str date;
            for (str option : index.options)
                if (option.starts_with("date "))
                    date = option.from(5);

            if (date != "") c += ", <i>" + date + "</i>";

            if (index.comment != "") s += "<br><br>"
                "<font color=#808080><i>" + 
                ::app::dict::mediae::canonical(index.comment) +
                "</i></font><br>&nbsp;";

            s += "<br><br>" + c;

            text.it_is_a_title = true;
            text.html = s;
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
            }
        }

        int clicked = 0;

        void on_notify (void* what) override
        {
            if (what == &text ) { clicked = text .clicked; notify(); }
        }
    };
}

