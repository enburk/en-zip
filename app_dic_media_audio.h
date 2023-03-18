#pragma once
#include "eng_parser.h"
#include "app_dic_html.h"
#include "app_dic_media.h"
namespace app::dic::audio
{
    struct player:
    widget<player>
    {
        html_view text;
        sfx::audio::player audio;
        sfx::media::medio medio;

        gui::time stay;
        gui::time start;
        sys::thread thread;
        media::index index;
        array<byte> audio_bytes;
        bool click = false;
        int  clicked = 0;
        str  html;

#define using(x) decltype(medio.x)& x = medio.x;
        using(mute)
        using(volume)
        using(loading)
        using(playing)
        using(resolution)
        using(duration)
        using(elapsed)
        using(status)
        using(error)
        #undef using

        ~player () { reset(); }

        void reset ()
        {
            try {
            thread.stop = true;
            thread.join();
            thread.check(); }
            catch (...) {}
            audio.reset();
            medio.done();
        }

        void prepare_html (array<str> links)
        {
            str title   = index.title;
            str sense   = index.sense;
            str credit  = index.credit;
            str comment = index.comment;

            title.replace_all("---", mdash);
            title.replace_all("--" , ndash);

            title = eng::parser::embolden(title, links);

            title   = media::canonical(title);
            credit  = media::canonical(credit);
            comment = media::canonical(comment);

            if (index.options.contains("sound"))
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
        }

        void load (media::index index_, array<str> links)
        {
            // if that same audio
            // is used for another word then do next:
            // actualize emboldened links,
            // and return

            using sfx::media::state;

            if (index == index_
            and status != state::loading
            and status != state::failed)
            {
                prepare_html(links);
                text.forbidden_links = links;
                text.html = html;
                return;
            }

            index = index_;

            reset();
            medio.load();
            thread = [this, links](std::atomic<bool>& cancel)
            {
                start = gui::time{};
                stay  = gui::time{1000 +
                index.title.size() * 40 +
                index.credit.size() * 10 +
                index.comment.size() * 20};

                prepare_html(links);
                text.forbidden_links = links;

                auto source = [](int source){
                    return "../data/media/storage." +
                    std::to_string(source) + ".dat"; };

                if (index != media::index{})
                    audio_bytes = sys::in::bytes(source(
                    index.location.source),
                    index.location.offset,
                    index.location.length);
            };
        }

        void play ()
        {
            if (medio.play()) {
                audio.play();
                logs::media << media::log(index);
                start = gui::time::now;
                stay.ms = stay.ms
                *150/100;
            }
        }
        void stop ()
        {
            if (medio.stop())
                audio.stop();
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
            if (what == &text )
            {
                clicked = text.clicked;
                click = true; notify();
                click = false;
            }

            using sfx::media::state;

            if (what == &loading
            and audio.status == state::finished
            and thread.done)
            {
                try
                {
                    thread.join();
                    thread.check();

                    text.html = std::move(html);

                    audio.load(std::move(
                    audio_bytes));    
                }
                catch (std::exception const& e) {
                medio.fail(e.what()); }
            }

            if (what == &loading
            and audio.status == state::ready
            and thread.done)
            {
                medio.stay();
            }

            if (what == &playing)
            {
                if (text.link != "")
                start = gui::time::now;
            }

            if (what == &playing
            and audio.status == state::finished
            and start + stay < gui::time::now)
            {
                medio.done();
            }

            if (what == &playing
            or  what == &loading)
            {
                if (audio.status ==
                state::failed)
                medio.fail(
                audio.error);
            }
 
            if (what == &volume)
                audio.volume =
                volume;

            if (what == &mute)
                audio.mute =
                mute;
        }
    };
}

