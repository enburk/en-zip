#pragma once
#include "app.h"
namespace studia::aux
{
    namespace report
    {
        struct report
        {
            str app, name;
            array<str> log;
            static inline array<report*> list;
            report (str app, str name) : app(app), name(name) { list += this; }
            void operator += (auto&& s) { log += std::forward<decltype(s)>(s); }
            path path () { return "../data/report/"+app+"_"+name+".txt"; }
            void load () { log = sys::optional_text_lines(path()); }
            void save () { truncate(1000); sys::write(path(), log); }
            int  size () { return log.size(); }
            void truncate (int nn)
            { 
                int n = size();
                if (n <= nn) return;
                log.resize(nn);
                log += bold(blue(
                "+" + str(n-nn) + " more"));
            }
        };

        void load () { for (auto r: report::list) r->load(); }
        void save () { for (auto r: report::list) r->save(); }
        void clear() { for (auto r: report::list) r->log.clear(); }

        namespace dic
        {
            report errors("dic", "errors");
            report usages("dic", "usages");
            report statts("dic", "statts");
        }
        namespace one
        {
            report errors("one", "errors");
            report anoma1("one", "anoma1");
            report anoma2("one", "anoma2");
            report anomal("one", "anomal");
            report duples("one", "duples");
            report orders("one", "orders");
            report audiom("res", "audiom");
            report audiop("res", "audiop");
            report audioq("res", "audioq");
            report videom("res", "videom");
            report videop("res", "videop");
            report videoq("res", "videoq");
            report wordsm("res", "wordsm");
        }
        namespace two
        {
            report errors("two", "errors");
            report anomal("two", "anomal");
            report audiom("two", "audiom");
            report wordsm("two", "wordsm");
        }
    }

    struct reports:
    widget<reports>
    {
        struct consolium:
        widget<consolium>
        {
            str link;
            widgetarium<gui::console> consoles;
            array<report::report*> reports;
            array<int> loaded;
            const int yes = 1;
            const int no = 0;

            void init (str app)
            {
                for (auto report : report::report::list)
                    if (report->app == app)
                    consoles.emplace_back(),
                    consoles.back().hide(),
                    reports += report,
                    loaded += no;
            }
            void on_change (void* what) override
            {
                if (what == &coord)
                {
                    for (auto& console: consoles)
                    console.coord = coord.now.local();
                    consoles.coord = coord.now.local();
                }

                for (auto& c: consoles)
                    if (what == &c.link)
                    link = c.link,
                    notify();
            }
            void reload ()
            {
                for (auto& c: consoles) c.clear();
                for (auto& x: loaded) x = no;
            }
            void prepare (int n)
            {
                if (loaded[n] == no) {
                    loaded[n] = yes;
                    reports[n]->load();
                    consoles[n] <<
                    reports[n]->log;
                }
            }
        };

        str link;
        gui::area<gui::selector> selector;
        gui::area<consolium> consolium;

        reports (str app)
        {
            consolium.object.init(app);
            for (auto r : report::report::list) if (r->app == app)
            selector.object.buttons.emplace_back(),
            selector.object.buttons.back().text.text = r->name;
            selector.object.selected = 0;
            reload();
        }

        void reload () { consolium.object.reload(); }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*13/10;
                selector .coord = xywh(0, 0, W, h);
                consolium.coord = xyxy(0, h, W, H);
            }
            if (what == &selector)
            {
                auto& consoles = 
                consolium.object.consoles;
                int n = selector.object.selected.now;
                for (int i=0; i<consoles.size(); i++)
                consoles[i].show(i == n);
                consolium.object.prepare(n);
            }
            if (what == &consolium)
            {
                link = consolium.object.link;
                notify();
            }
            if (what == &alpha && shown())
            {
                int n =
                selector.object.selected.now;
                consolium.object.prepare(n);
            }
        }
    };
}

