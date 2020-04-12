#pragma once
#include "app.h"
#include <future>
#include "studio_compile_dictionary.h"
namespace studio::compile
{
    using namespace std::literals::chrono_literals;

    struct studio : gui::widget<studio>
    {
        gui::canvas canvas;
        gui::area<gui::text::page> console_out; array<str> out;
        gui::area<gui::text::page> console_err; array<str> err;

        std::mutex mutex;
        std::future<void> compilation;
        gui::property<gui::time> timer;
        bool data_updated = false;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w;
                int H = coord.now.h;

                canvas.coord = XYWH(0, 0, W, H);

                console_out.coord = XYWH(0,   0, W/2, H);
                console_err.coord = XYWH(W/2, 0, W/2, H);
            }
            if (what == &skin)
            {
                canvas.color = gui::skins[skin.now].light.back_color;
                console_out.object.view.ground.color = gui::skins[skin.now].white;
                console_err.object.view.ground.color = gui::skins[skin.now].white;
                console_out.object.alignment = XY{gui::text::left, gui::text::top};
                console_err.object.alignment = XY{gui::text::left, gui::text::top};
            }
            if (what == &timer && compilation.valid())
            {
                auto status = compilation.wait_for(0ms);
                if (status == std::future_status::ready)
                {
                    compilation.get();
            
                    timer.go (gui::time(), gui::time()); /// stop timer
                 /// this statement will cause recursive on_change()
                 /// so do it after std::future get ivalid
                 /// to prevent second get() call
           
                    notify();
                }

                std::lock_guard guard{mutex};
                console_out.object.text = str(out);
                console_err.object.html = str(err);
                console_out.object.scroll.y.top = max<int>();
                console_err.object.scroll.y.top = max<int>();
            }
        }

        void run ()
        {
            if (compilation.valid()) return;

            out.clear();
            err.clear();

            timer.go (gui::time::infinity,
                      gui::time::infinity);

            compilation = std::async([this]()
            {
                data_updated = false;

                if (dictionary_update(out, err))
                    data_updated = true;
            });
        }
    };
}

