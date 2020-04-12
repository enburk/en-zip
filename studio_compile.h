#pragma once
#include "app.h"
#include <future>
#include "studio_compile_dictionary.h"
namespace studio::compile
{
    using namespace std::literals::chrono_literals;

    struct studio : gui::widget<studio>
    {
        gui::area<gui::text::console> out;
        gui::area<gui::text::console> err;

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
                if (status == std::future_status::ready)
                {
                    try { compilation.get(); }

                    catch (const std::exception & e) {
                        out.object << "<b><font color=#B00020>" "exception: " +
                            str(e.what()) + "</font></b>";
                    }
                    catch (...) {
                        out.object << "<b><font color=#B00020>"
                            "unidentified exception" "</font></b>";
                    }
            
                    timer.go (gui::time(), gui::time()); /// stop timer
                 /// this statement will cause recursive on_change()
                 /// so do it after std::future get ivalid
                 /// to prevent second get() call
           
                    notify();
                }
            }
        }

        void run ()
        {
            if (compilation.valid()) return;

            out.object.clear();
            err.object.clear();

            timer.go (gui::time::infinity,
                      gui::time::infinity);

            compilation = std::async([this]() -> void
            {
                data_updated = false;

                if (dictionary_update(out.object, err.object))
                    data_updated = true;

                setlocale(LC_ALL,"en_US.utf8");

                auto resources = media::scan("../datae", out.object, err.object);

                out.object << (data_updated ?
                    "<b><font color=#800000>" "data updated" "</font></b>":
                    "<b><font color=#000080>"  "up to date"  "</font></b>");
            });
        }
    };
}

