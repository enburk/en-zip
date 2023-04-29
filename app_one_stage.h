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
        property<bool> playmode = false;
        property<bool> translated = false;
        int current = 0;
        int Height = 0;
        int height = 0;

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
        unit* theme = nullptr;

        void fill ()
        {
            theme = where;

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
                        e.new_topic = false;
                        e.new_chain = false;
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
            height = 0;
            Height = 0;
            xy size = coord.now.local().size;
            int w = gui::metrics::text::height*15;
            int h = gui::metrics::text::height*18;
            if (w <= 0 or size.x <= 0) return;
            if (h <= 0 or size.y <= 0) return;
            w = size.x / (size.x / w);
            h = size.y / (size.y / h);
            height = h;
            
            int i = 0; xy p; int y = 0;

            auto nextslide = [&]()
            {
                slide& s =
                slides[i++];
                s.reset();
                s.coord = xywh(p.x, p.y, w, h); Height = p.y + h;
                p.x += w; if (p.x + w > size.x)
                p.y += h, p.x = 0;
                y = 0;
            };

            for (auto& entry: entries)
            {
                bool was_pixed = i == 0 or not
                slides[i-1].entries.empty() and
                slides[i-1].entries.back()->pixed;

                int hh = entry.resize(w, h);
                if (hh + y > h or i == 0
                or  entry.pixed != was_pixed
                or  entry.new_chain)
                    nextslide();

                entry.coord = xywh(
                slides[i-1].coord.now.x,
                slides[i-1].coord.now.y + y, w, hh);
                slides[i-1].entries += &entry;

                y += hh;
            }

            slides.truncate(i);
            slides .coord = coord.now.local();
            entries.coord = coord.now.local();

            int H = min(h, w);

            for (auto& s: slides)
            {
                s.mute = mute;
                s.volume = volume;

                int hh = 0;
                bool pixed = false;

                for (auto& e: s.entries)
                hh += e->coord.now.h,
                pixed |= e->pixed;

                if (pixed) continue;
                if (hh>=H) continue;

                for (auto& e: s.entries)
                e->shift(xy(0, H/2-hh/2));
            }
        }

        void load ()
        {
            medio.stay();
        }

        void play ()
        {
            if (entries.empty())
                fill();

            if (playmode.now)
            {
                medio.play();
                playslide();
            }
            else
            {
                medio.stay();
                current = slides.size()-1;
                for (slide& s: slides)
                s.show();
            }
        }

        void stop ()
        {
            if (medio.stop()
            and not slides.empty())
            slides[current].stop();
        }

        void playslide ()
        {
            if (
            slides.empty()) return;
            slides[current].play();
            scroll(-height);
        }

        void showslide ()
        {
            if (
            slides.empty()) return;
            slides[current].show();
            scroll(-height);
        }

        void hideslide ()
        {
            if (
            slides.empty()) return;
            slides[current].hide();
            scroll(0);
        }

        void prev ()
        {
            hideslide();
            if (current == 0) return;
            current--;
            scroll(0);
        }

        void next ()
        {
            showslide();
            if (current >= slides.size()-1) return;
            current++;
            if (status != state::playing)
            showslide();
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
                    playslide();
                    break;
                case state::finished:
                    if (current >= slides.size()-1) {
                        medio.done();
                        break;
                    }
                    current++;
                    playslide();
                    break;
                default:
                    break;
                }
            }

            if (what == &translated)
                fill();

            if (what == &volume)
                for (slide& s: slides)
                    s.volume = volume;

            if (what == &mute)
                for (slide& s: slides)
                    s.mute = mute;
        }

        void scroll (int delta)
        {
            int top =
            slides.empty() ? 0 :
            slides.front().coord.to.y;

            int bottom =
            slides.empty() ? 0 :
            slides[current].coord.to.y +
            slides[current].coord.to.h;

            int h = bottom - top;

            int
            newtop = top + delta;
            newtop = min(0, max(newtop,
            coord.to.h - h));

            delta = newtop - top;

            for (auto& e: entries) e.shift(xy{0,delta}, 500ms);
            for (auto& s: slides ) s.shift(xy{0,delta}, 500ms);
        }

        bool on_mouse_wheel (xy, int delta) override
        {
            scroll(delta < 0 ? -height : height);
            return true;
        }
    };
}