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
            static inline std::map<str, std::map<str, report*>> map;
            report (str app, str name) : app(app), name(name) { map[app][name] = this; }
            void operator += (auto&& s) { log += std::forward<decltype(s)>(s); }
            path path () { return "../data/report/"+app+"_"+name+".txt"; }
            void load () { log = sys::optional_text_lines(path()); }
            void save () { sys::write(path(), log); }
            int  size () { return log.size(); }
            void truncate (int nn)
            { 
                int n = size();
                log.resize(nn);
                log += bold(blue(
                "+" + str(n-nn) + " more"));
            }
        };

        void load (str app) { for (auto& [name, report]: report::map[app]) report->load(); }
        void save (str app) { for (auto& [name, report]: report::map[app]) report->save(); }
        void clear(str app) { for (auto& [name, report]: report::map[app]) report->log.clear(); }

        namespace one
        {
            report errors("one", "errors");
            report anoma1("one", "anoma1");
            report anoma2("one", "anoma2");
            report anomal("one", "anomal");
            report duples("one", "duples");
            report orders("one", "orders");
            report audiom("one", "audiom");
            report audiop("one", "audiop");
            report audioq("one", "audioq");
            report videom("one", "videom");
            report videop("one", "videop");
            report videoq("one", "videoq");
            report wordsm("one", "wordsm");
        }
    }

/*

    namespace report
    {
        array<str> errors;
        array<str> usages;
        array<str> statts;
        void load ()
        {
            std::filesystem::path dir = "../data/report";
            errors = sys::optional_text_lines(dir/"dic_errors.txt");
            usages = sys::optional_text_lines(dir/"dic_usages.txt");
            statts = sys::optional_text_lines(dir/"dic_statts.txt");
            usages.resize(1000);
        }
        void save ()
        {
            std::filesystem::path dir = "../data/report";
            sys::write(dir/"dic_errors.txt", errors);
            sys::write(dir/"dic_usages.txt", usages);
            sys::write(dir/"dic_statts.txt", statts);
        }
        void clear()
        {
            errors.clear();
            usages.clear();
            statts.clear();
        }
    }

    namespace report
    {
        struct report;
        std::map<str, report*> map;
        struct report
        {
            str name;
            array<str> log;
            report (str name) : name(name) { map[name] = this; }
            void operator += (auto&& s) { log += std::forward<decltype(s)>(s); }
            path file () { return "../data/report/one_"+name+".txt"; }
            void load () { log = sys::optional_text_lines(file()); }
            void save () { sys::write(file(), log); }
        };

        report errors("errors");
        report anoma1("anoma1");
        report anoma2("anoma2");
        report anomal("anomal");
        report duples("duples");
        report orders("orders");
        report audiom("audiom");
        report audiop("audiop");
        report audioq("audioq");
        report videom("videom");
        report videop("videop");
        report videoq("videoq");
        report wordsm("wordsm");

        void load () { for (auto& [name, report]: map) report->load(); }
        void save () { for (auto& [name, report]: map) report->save(); }
        void clear() { for (auto& [name, report]: map) report->log.clear(); }
    }


    struct reports:
    widget<reports>
    {
        struct consobar:
        widget<consobar>
        {
            str link;
            gui::console errors;
            gui::console duples;
            gui::console orders;
            gui::console anoma1;
            gui::console anoma2;
            gui::console anomal;
            array<gui::console*> consoles;
            array<report::report*> reports;
            array<int> readiness;
            consobar ()
            {
                consoles += &errors; reports += &report::errors; readiness += 1;
                consoles += &duples; reports += &report::duples; readiness += 1;
                consoles += &orders; reports += &report::orders; readiness += 1;
                consoles += &anoma1; reports += &report::anoma1; readiness += 1;
                consoles += &anoma2; reports += &report::anoma2; readiness += 1;
                consoles += &anomal; reports += &report::anomal; readiness += 1;
                for (auto& c: consoles)
                c->hide();
            }
            void on_change (void* what) override
            {
                if (what == &coord)
                for (auto& c: consoles)
                c->coord = coord.now.local();

                for (auto& c: consoles)
                if (what == &c->link)
                link = c->link,
                notify();
            }
            void reload ()
            {
                for (auto& c: consoles) c->clear();
                for (auto& r: readiness) r = 0;
            }
            void prepare (int n)
            {
                if (readiness[n] == 0) {
                    readiness[n] = 1;
                    reports[n]->load();
                   *consoles[n] <<
                    reports[n]->log;
                }
            }
        };

        str link;
        gui::area<gui::selector> selector;
        gui::area<consobar> consobar;

        reports ()
        {
            int i = 0;
            auto& sel = selector.object;
            sel.buttons(i++).text.text = "errors";
            sel.buttons(i++).text.text = "duplicates";
            sel.buttons(i++).text.text = "ordering";
            sel.buttons(i++).text.text = "anomal: duples";
            sel.buttons(i++).text.text = "anomal: option";
            sel.buttons(i++).text.text = "anomal";
            sel.selected = 0;
            reload();
        }

        void reload () { consobar.object.reload(); }

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
                int l = gui::metrics::line::width;
                selector.coord = xywh(0, 0, W, h);
                consobar.coord = xyxy(0, h, W, H);
            }
            if (what == &selector)
            {
                auto& consoles = 
                consobar.object.consoles;
                int n = selector.object.selected.now;
                for (int i=0; i<consoles.size(); i++)
                consoles[i]->show(i == n);
                consobar.object.prepare(n);
            }
            if (what == &consobar)
            {
                link = consobar.object.link;
                notify();
            }
            if (what == &alpha && shown())
            {
                int n =
                selector.object.selected.now;
                consobar.object.prepare(n);
            }
        }
    };
*/
}

