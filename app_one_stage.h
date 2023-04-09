#pragma once
#include "app.h"
#include "app_one_slide.h"
namespace app::one
{
    struct stage:
    widget<stage>
    {
        sfx::media::medio medio;
        widgetarium<slide> slides;
        widgetarium<entry> entries;
        property<bool> translated = false;
        int current = 0;

        using state = sfx::media::state;

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

        using unit = content::unit;

        unit* where = nullptr;

        void fill ()
        {
            where = course.find(
            sys::settings::load(
            "app:one::path", ""));

            where = &course.root.units[0].units[1];

            content::unit* theme = where;

            while ( theme
                and theme->kind != unit::theme) theme =
                    theme->parent;

            while ( theme and
                not theme->units.empty()
                and theme->units.front().kind == unit::theme) theme =
                   &theme->units.front();

            if (not theme) return;

            int i = 0;

            for (unit& topic: theme->units)
            {
                if (topic.kind != unit::topic) continue;

                bool new_topic = true;

                for (unit& chain: topic.units)
                {
                    if (chain.kind != unit::chain) continue;

                    bool new_chain = true; int j = i;

                    for (unit& leaf: chain.units)
                    {
                        if (leaf.kind != unit::leaf) continue;

                        auto& e = entries[i++];
                        e.translated = translated;
                        e.number = leaf.entry;
                        e.hide();
                        e.load();
                    }

                    entries.stable_partition(j, i,
                    [](auto& e){ return e->pixed; });

                    entries[j].new_topic = new_topic;
                    entries[j].new_chain = new_chain;
                    new_topic = false;
                    new_chain = false;
                }
            }

            entries.truncate(i);

            current = 0;
            resize();
            load();
        }

        void resize ()
        {
            xy size = coord.now.local().size;
            int w = gui::metrics::text::height*14;
            int h = gui::metrics::text::height*18;
            w = size.x / (size.x / w);
            h = size.y / (size.y / h);
            
            int i = 0; xy p; int y = 0;

            auto nextslide = [&]()
            {
                slide& s =
                slides[i++];
                s.reset();
                s.coord = xywh(p.x, p.y, w, h);
                p.x += w; if (p.x > size.x)
                p.y += h, p.x = 0;
                y = 0;
            };

            nextslide();

            for (auto& entry: entries)
            {
                bool was_pixed = not
                slides[i-1].entries.empty() and
                slides[i-1].entries.back()->pixed;

                int hh = entry.resize(w, h);
                if (hh + y > h or entry.new_chain
                or  entry.pixed != was_pixed)
                    nextslide();

                entry.coord = xywh(p.x-w, p.y+y, w, hh);

                slides[i-1].entries += &entry;

                y += hh;
            }

            slides.truncate(i);
            slides .coord = coord.now.local();
            entries.coord = coord.now.local();
        }

        void load ()
        {
            medio.stay();
        }

        void play ()
        {
            if (entries.empty())
                fill();

            medio.play();
        }

        void stop ()
        {
            if (medio.stop()
            and not slides.empty())
            slides[current].stop();
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
                resize();

            if (what == &playing)
            {
                if (not slides.empty())
                switch(slides[current].status) {
                case state::ready:
                case state::paused:
                    slides[current].show();
                    slides[current].play();
                    break;
                case state::finished:
                    if (current >= slides.size()-1) {
                        medio.done();
                        break;
                    }
                    current++;
                    slides[current].show();
                    slides[current].play();
                    break;
                default:
                    break;
                }
            }

            if (what == &volume)
                for (slide& s: slides)
                    s.volume = volume;

            if (what == &mute)
                for (slide& s: slides)
                    s.mute = mute;
        }
    };
}