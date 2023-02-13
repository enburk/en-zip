#pragma once
#include "app.h"
#include <future>
#include "studio_build_dic.h"
#include "studio_build_dic+.h"
#include "studio_build_one.h"
namespace studio::build
{
    using namespace std::literals::chrono_literals;

    struct studio : gui::widget<studio>
    {
        gui::area<gui::console> out_area;
        gui::area<gui::console> err_area;
        gui::console& out = out_area.object;
        gui::console& err = err_area.object;

        sys::thread compilation;
        gui::property<gui::time> timer;
        std::atomic<bool> data_updated = false;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;
                out_area.coord = xywh(0,   0, W/2, H);
                err_area.coord = xywh(W/2, 0, W/2, H);
            }

            if (what == &timer and compilation.done)
            {
                try { compilation.check(); }

                catch (std::exception const& e) {
                err << bold(red("exception: " +
                str(e.what()))); }

                timer.go(
                gui::time{},
                gui::time{});

                notify();
            }
        }

        void run ()
        {
            if (timer.to != gui::time{}) return;

            out.clear();
            err.clear();
            out.limit = 1024*1024;
            err.limit = 1024*1024;

            timer.go(
            gui::time::infinity,
            gui::time::infinity);

            compilation = [this](auto& stop)
            {
                app::logs::report = out;
                app::logs::errors = err;

                data_updated = dic::update();

                eng::
                vocabulary
                vocabulary("../data/vocabulary.dat");
                if (not eng::unittest::smoke(vocabulary, out))
                    return;

                array<int> redirects;
                redirects.resize(vocabulary.size());
                sys::in::pool pool("../data/dictionary_indices.dat");
                for (int i=0; i<vocabulary.size(); i++) {
                    eng::dictionary::index index; index << pool;
                    redirects[i] = index.redirect; }

                media::logs::out = out;
                media::logs::err = err;
                media::out::data mediadata;

                dic::compile(vocabulary, redirects, mediadata);
                one::compile(vocabulary, redirects, mediadata);
            //  two::compile(vocabulary, redirects, mediadata);

                mediadata.save();

                data_updated =
                data_updated or
                media::report::updated;
                out << (data_updated?
                bold(yellow("UPDATED")):
                bold(green("UP TO DATE")));
            };
        }
    };
}

