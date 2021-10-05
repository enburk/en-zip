#pragma once
#include "app.h"
#include <future>
#include "media_scan.h"
#include "studio_build_dic.h"
#include "studio_build_dic_.h"
namespace studio::build
{
    using namespace std::literals::chrono_literals;

    struct studio : gui::widget<studio>
    {
        gui::area<gui::console> out;
        gui::area<gui::console> err;

        std::future<void> compilation;
        gui::property<gui::time> timer;
        bool data_updated = false;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;

                out.coord = XYWH(0,   0, W/2, H);
                err.coord = XYWH(W/2, 0, W/2, H);
            }
            if (what == &timer && compilation.valid())
            {
                auto status = compilation.wait_for(0ms);
                if (status != std::future_status::ready)
                    return;

                try { compilation.get(); }

                catch (const std::exception & e) {
                    out.object << bold(red("exception: " +
                        str(e.what())));
                }
                catch (...) {
                    out.object << bold(red(
                        "unidentified exception"));
                }
            
                timer.go (gui::time(), gui::time()); /// stop timer
                /// this statement will cause recursive on_change()
                /// so do it after std::future gets invalid
                /// to prevent second get() call
           
                notify();
            }
        }

        void run ()
        {
            if (compilation.valid()) return;

            out.object.clear();
            err.object.clear();
            out.object.limit = 1024*1024;
            err.object.limit = 1024*1024;

            timer.go (gui::time::infinity,
                      gui::time::infinity);

            compilation = std::async(std::launch::async, [this]() -> void
            {
                try
                {
                    data_updated = false;

                    if (dictionary_update(out.object, err.object))
                        data_updated = true;

                    eng::vocabulary vocabulary("../data/vocabulary.dat");
                    if (not eng::unittest::proceed(vocabulary, out.object))
                        return;

                    setlocale(LC_ALL,"en_US.utf8");

                    media::report::out = &out.object;
                    media::report::err = &err.object;
                    media::report::unidentified.clear();
                    media::report::id2path.clear();
                    media::report::data_updated = false;

                    auto resources = media::scan::scan("../datae");

                    if (media::report::unidentified.size() > 0) {
                        err.object << "unidentified files:";
                        for (auto path : media::report::unidentified)
                            err.object << path.string();
                    }

                    for (auto & [id, paths] : media::report::id2path) {
                        if (paths.size () > 1) {
                            err.object << "files with same id: " + id;
                            for (auto path : paths)
                                err.object << path.string();
                        }
                    }

                    dic::compile(resources, out.object);

                    data_updated |=
                    media::report::data_updated;
                    out.object << (data_updated?
                      bold(yellow("data updated")):
                        bold(blue("up to date")));
                }
                catch (std::exception & e) {
                    out.object << bold(red(
                        e.what()));
                }
                catch (...) {
                    out.object << bold(red(
                        "unknown exception"));
                }
            });
        }
    };
}

