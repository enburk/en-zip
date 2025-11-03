#pragma once
#include "app.h"
#include <future>
#include "studio_build_dic.h"
#include "studio_build_dic+.h"
#include "studio_build_one.h"
namespace studio::build
{
    struct studio:
    widget<studio>
    {
        gui::area<gui::console> out_area;
        gui::area<gui::console> err_area;
        gui::console& out = out_area.object;
        gui::console& err = err_area.object;

        sys::thread compilation;
        gui::property<gui::time> timer;
        std::atomic<bool> dic_updated = false;
        std::atomic<bool> app_updated = false;

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

                timer.go({},{});
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

            dic_updated = false;
            app_updated = false;

            timer.go(
            gui::time::infinity,
            gui::time::infinity);

            compilation = [this](auto& stop)
            {
                app::logs::report = out;
                app::logs::errors = err;

                dic_updated = dic::update();
                if (dic_updated)
                    return;

                if (not eng::unittest::smoke(app::vocabulary, out))
                    return;

                media::logs::out = out;
                media::logs::err = err;
                media::out::data mediadata;

                dic::compile(mediadata);
                one::compile(mediadata);
            //  two::compile(mediadata);

                for (auto r: mediadata.unsquared)
                    dic::report::errors +=
                    linked(red("unsquared: ") +
                    str(r->path), "file://" +
                    str(r->path));

                dic::reportage();

                mediadata.save();

                app_updated = true;
            };
        }
    };
}

