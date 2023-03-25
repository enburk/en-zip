#pragma once
#include "app_dic.h"
namespace studio::dic
{
    using namespace std::filesystem;

    struct detail:
    widget<detail>
    {
        path datpath;
        path txtpath;

        app::dic::html_view filepath;
        gui::text::editor   filename;
        app::dic::html_view keywords;
        gui::text::editor   textfile;
        app::dic::html_view preview;

        gui::radio::group crops;
        app::dic::video::player orig;
        app::dic::video::player crop;
        app::dic::video::player qrop;
        gui::image shadow;

        int clicked = 0;

        void select (str info)
        {
            filename.text = "";
            textfile.text = "";
            keywords.text = "";
            preview .text = "";

            datpath = info.c_str();
            txtpath = info.c_str();
            txtpath.replace_extension(".txt");

            if (not exists(datpath)) {
            filepath.html = red(info);
            return; }

            str s = info;
            s.replace_all("\\", "/");
            if (s.starts_with("../"   )) s = s.from(3);
            if (s.starts_with("datae/")) s = s.from(6);
            auto ss = s.split_by("/");

            s = ""; for (str x: ss)
            s += light("> ") + dark(x) + "<br>";

            filepath.html = s;
            filename.text = datpath.filename().string();

            //if (exists(txtpath))
            //textfile.text = dat::in::file(txtpath).pool.red(j->second);
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int w = gui::metrics::text::height*5;
                int h = gui::metrics::text::height*12/10;
                int p = H/100;
                int y = 0;

                filepath.coord = xywh(0, y, W, 10*p); y += 10*p;
                filename.coord = xywh(0, y, W, 10*p); y += 10*p;
                keywords.coord = xywh(0, y, W, 15*p); y += 15*p;
                textfile.coord = xywh(0, y, W, 30*p); y += 30*p;
                preview .coord = xywh(0, y, W, 30*p); y += 30*p;
            }
            if (what == &skin)
            {
                auto s = gui::skins[skin];
                auto l = gui::metrics::line::width;

                filepath.alignment = xy{pix::left, pix::top};
                filename.canvas.color = s.ultralight.first;
                textfile.canvas.color = s.ultralight.first;
                filename.padding = xyxy{2*l, 2*l, 2*l, 2*l};
                textfile.padding = xyxy{2*l, 2*l, 2*l, 2*l};
                filename.view.frame.thickness = l;
                textfile.view.frame.thickness = l;
            }
            if (what == &filepath) { clicked = filepath.clicked; notify(); }
            if (what == &keywords) { clicked = keywords.clicked; notify(); }
            if (what == &preview ) { clicked = preview .clicked; notify(); }

            if (what == &filename.text)
            {
                //doc::text::repo::edit(txtpath);
            }

            if (what == &textfile.text)
            {
                //doc::text::repo::edit(txtpath);
            }

        }
    };
}
