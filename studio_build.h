#pragma once
#include "app.h"
#include <future>
#include "studio_app_dict.h"
#include "studio_build_dic.h"
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
                    out.object << "<b><font color=#B00020>" "exception: " +
                        str(e.what()) + "</font></b>";
                }
                catch (...) {
                    out.object << "<b><font color=#B00020>"
                        "unidentified exception" "</font></b>";
                }
            
                timer.go (gui::time(), gui::time()); /// stop timer
                /// this statement will cause recursive on_change()
                /// so do it after std::future get invalid
                /// to prevent second get() call
           
                notify();
            }
        }

        void run ()
        {
            if (compilation.valid()) return;

            out.object.clear();
            err.object.clear();

            timer.go (gui::time::infinity,
                      gui::time::infinity);

            compilation = std::async(std::launch::async, [this]() -> void
            {
                try
                {
                    data_updated = false;

                    if (dictionary_update(out.object, err.object))
                        data_updated = true;

                    setlocale(LC_ALL,"en_US.utf8");

                    media::report::out = &out.object;
                    media::report::err = &err.object;
                    media::report::unidentified.clear();
                    media::report::id2path.clear();
                    media::report::data_updated = false;

                    auto resources = media::scan("../datae");

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

                    ::studio::dic::compile(resources);

                    data_updated |=
                    media::report::data_updated;
                    out.object << (data_updated ?
                        "<b><font color=#800000>" "data updated" "</font></b>":
                        "<b><font color=#000080>"  "up to date"  "</font></b>");
                }
                catch (std::exception & e) {
                    out.object << "<b><font color=#B00020>" +
                        str(e.what()) + "</font></b>";
                }
                catch (...) {
                    out.object << "<b><font color=#B00020>"
                        "unknown exception" "</font></b>";
                }
            });
        }
    };
}

