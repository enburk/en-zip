#pragma once
#include "app.h"
#include "app_two_slide.h"
namespace app::two
{
    struct stage:
    widget<stage>
    {
        sfx::media::medio medio;
        widgetarium<slide> slides;
        widgetarium<entry> entries;
        property<bool> playmode = false;
        property<bool> translated = false;
        int clicked = 0;
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

        unit* topic = nullptr;

        void go (unit* t)
        {
            if (not t or t == topic) return;
            topic = t;

            if (topic->kind == unit::theme)
            if (auto theme = topic->first_theme())
                topic = theme->first_topic();

            fill();

            bool found = false;
            for (int i=0; i<slides.size(); i++)
            {
                if (not found) current = i;
                if (playmode.now and found)
                    slides[i].hide(); else
                    slides[i].show();
                if (slides[i].topic == topic)
                    found = true;
            }

            int y =
            slides.empty() ? 0 :
            slides[current].coord.to.y;
            for (auto& e: entries) e.shift(xy{0,-y}, 500ms);
            for (auto& s: slides ) s.shift(xy{0,-y}, 500ms);
        }

        void fill ()
        {
            if (not topic
            or  not topic->kind == unit::topic)
                return;

            auto theme = topic->parent;

            if (not theme
            or  not theme->kind == unit::theme)
                return;

            entries.clear();

            for (unit& topic: theme->units)
            {
                if (topic.kind != unit::topic) continue;

                for (unit& chain: topic.units)
                {
                    if (chain.kind != unit::chain) continue;

                    for (unit& leaf: chain.units)
                    {
                        if (leaf.kind != unit::leaf) continue;

                        if (not translated)
                        if (course.entries[leaf.entry].eng == ""
                        or  course.entries[leaf.entry].opt.
                            external.contains("Ru"))
                            continue;

                        auto& e = entries.emplace_back();
                        e.translated = translated;
                        e.number = leaf.entry;
                        e.topic = &topic;
                        e.hide();
                    }
                }
            }

            current = 0;
            resize();
            load();

            entries.show();
        }

        void resize ()
        {
            height = 0;
            Height = 0;
            xy size = coord.now.local().size;
            int w = gui::metrics::text::height*15;
            int h = gui::metrics::text::height*18;
            int d = gui::metrics::text::height/2;
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

                auto [ww, hh] = entry.resize_to_fit(w, h);
                if (hh + y > h - d or i == 0
                or  slides[i-1].entries.size() >= 2
                or  entry.pixed != was_pixed)
                    nextslide();

                if (not entry.pixed) y += d;

                entry.coord.go(xywh(
                slides[i-1].coord.now.x,
                slides[i-1].coord.now.y + y, ww, hh));//, 500ms);
                slides[i-1].entries += &entry;

                y += hh;
            }

            slides.truncate(i);
            slides .coord = coord.now.local();
            entries.coord = coord.now.local();

            int H = min(h, w);

            unit* last_topic = nullptr;

            for (auto& s: slides)
            {
                s.topic =
                s.entries.empty() ? last_topic :
                s.entries.front()->topic;

                if (not s.topic)
                    std::abort();

                last_topic = s.topic;

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
                e->shift(xy(0, H/2-hh/2));//, 500ms);
            }
        }

        void load ()
        {
            medio.stay();
        }

        void show_all ()
        {
            medio.stay();

            if (entries.empty())
                fill();

            if (slides.empty())
                return;

            topic = slides.front().topic;
            current = slides.size()-1;
            for (slide& s: slides)
                s.show();
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
                resize();

            if (what == &entries)
            {
                auto entry = entries.notifier;
                xy origin0 = entry->coord.to.origin;
                xy shift = sys::mouse::position() - origin0;

                clicked = entry->clicked;
                notify();

                xy origin1 = entry->coord.to.origin;
                if (origin1.y < 0 or origin1.y >= coord.now.h/height*height)
                scroll((origin0.y - origin1.y)/height*height, 0ms);

                xy origin2 = entry->coord.to.origin;
                if (origin2.y >= coord.now.h/height*height)
                scroll(-height, 0ms);

                xy origin3 = entry->coord.to.origin;
                sys::mouse::position(shift + origin3);
            }

            if (what == &translated
            // prevent fill on start
            and not entries.empty())
                fill();

            if (what == &volume)
                for (slide& s: slides)
                    s.volume = volume;

            if (what == &mute)
                for (slide& s: slides)
                    s.mute = mute;
        }

        void scroll (int delta, gui::time time = 500ms)
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

            for (auto& e: entries) e.shift(xy{0,delta}, time);
            for (auto& s: slides ) s.shift(xy{0,delta}, time);
        }

        bool on_mouse_wheel (xy, int delta) override
        {
            scroll(delta < 0 ? -height : height);
            return true;
        }
    };
}