#pragma once
#include "studia_dic_detail+.h"
namespace studio::dic
{
    struct detail:
    widget<detail>
    {
        path datpath;
        path txtpath;

        using text = app::dic::html_view;
        using edln = gui::text::one_line_editor;
        using edit = gui::text::editor;
        using Edln = gui::area<edln>;
        using Edit = gui::area<edit>;

        text filepath;
        Edln Filename; edln& filename = Filename.object;
        Edit Textfile; edit& textfile = Textfile.object;

        Mediadetail mediadetail;
        gui::button button_delete;
        gui::button button_revert;
        gui::timer  editing;

        std::unordered_map<path, path> renames;
        str filename_text;
        str textfile_text;
        int clicked = 0;

        detail () { select(""); }
       ~detail () { select(""); }

        void save () try
        {
            str s;
            s = textfile.text; s.trimr();
            if (textfile_text != s) {
                textfile_text  = s;

                if (s == ""
                and exists(txtpath))
                {
                    path dir = txtpath.
                    parent_path()/".del";
                    create_directories(dir);
                    rename(txtpath, dir/
                    txtpath.filename());
                }
                else
                sys::out::write(txtpath, s.lines());
                // crop can be changed
                select(str(datpath));
            }

            s = filename.text; s.strip();
            if (filename_text != s) {
                filename_text  = s;

                if (s == "") return;
                path dir = datpath.parent_path();
                path datpath_ = dir/str2path(s + ".dat");
                path txtpath_ = dir/str2path(s + ".txt");
                datpath_.replace_extension(datpath.extension());
                if (exists(datpath_)) throw std::runtime_error(
                "already exists: " + str(datpath_.filename()));
                rename(datpath, datpath_); if (exists(txtpath))
                rename(txtpath, txtpath_);
                renames[datpath] = datpath_;
                // crop can be changed
                select(str(datpath_));
            }
        }
        catch (std::exception const& e)
        {
            filepath.html = str(
            filepath.html) + "<br>" +
            red(bold(aux::unicode::
            what(e.what())));
        }

        void select (str info) try
        {
            save();

            datpath = str2path(info);

            std::unordered_set<path> anticycle;
            while (not exists(datpath))
            {
                anticycle.emplace(datpath);

                if (auto
                it =  renames.find(datpath);
                it == renames.end()) break;
                else datpath = it->second;

                if (anticycle.contains(datpath))
                break;
            }

            txtpath = datpath;
            txtpath.replace_extension(".txt");

            mediadetail.select(datpath, txtpath);

            str s = info;
            s.replace_all("\\", "/");
            if (s.starts_with("../"   )) s = s.from(3);
            if (s.starts_with("datae/")) s = s.from(6);
            
            str tree = "";
            for (str x: s.split_by("/"))
            tree += gray(">> ") + x + "<br>";

            filepath.html = tree;
            filename_text = str(datpath.stem());
            textfile_text = sys::in::optional_text(txtpath);
            filename.text = filename_text;
            textfile.text = textfile_text;
            filename.read_only = true;
            textfile.read_only = true;

            button_delete.hide();
            button_revert.hide();

            if (info == "") return;
            if (not exists(datpath))
            {
                filepath.html = red(bold(info));
                path dir = datpath.parent_path() / ".del";
                bool del = exists(dir / datpath.filename());
                button_revert.show(del);
                button_delete.hide();
                return;
            }

            button_delete.show();
            button_revert.hide();

            filename.read_only = false;
            textfile.read_only = false;
        }
        catch (std::exception const& e)
        {
            filepath.html = str(
            filepath.html) + "<br>" +
            red(bold(aux::unicode::
            what(e.what())));
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
                Filename.coord = xywh(0, y, W,  6*p); y +=  6*p;
                Textfile.coord = xywh(0, y, W, 30*p); y += 30*p;

                button_delete.coord = xywh(0, H-h, W/6, h);
                button_revert.coord = xywh(0, H-h, W/6, h);

                mediadetail.coord = xyxy(0, y, W, H);
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
                Filename.show_focus = true;
                Textfile.show_focus = true;
                filename.wordwrap   = true;

                button_delete.text.html = red("delete");
                button_revert.text.html = red("revert");
            }
            if (what == &filepath) {
                clicked = filepath.clicked;
                notify(); }

            if (what == &filename.update_text
            or  what == &textfile.update_text)
            {
                editing.setup(5s);
            }
            if (what == &editing)
            {
                save();
            }

            if (what == &mediadetail)
            {
                filename.text =
                mediadetail.resource.text();
                save();
            }

            if (what == &button_delete) try
            {
                path dir = datpath.parent_path()/".del";
                std::filesystem::create_directories(dir);
                path datpath_ = dir / datpath.filename();
                path txtpath_ = dir / txtpath.filename();
                rename(datpath, datpath_); if (exists(txtpath))
                rename(txtpath, txtpath_);
                select(str(datpath));
            }
            catch (std::exception const& e)
            {
                filepath.html = str(
                filepath.html) + "<br>" +
                red(bold(aux::unicode::
                what(e.what())));
            }
            if (what == &button_revert) try
            {
                path dir = datpath.parent_path()/".del";
                path datpath_ = dir / datpath.filename();
                path txtpath_ = dir / txtpath.filename();
                rename(datpath_, datpath); if (exists(txtpath_))
                rename(txtpath_, txtpath);
                select(str(datpath));
            }
            catch (std::exception const& e)
            {
                filepath.html = str(
                filepath.html) + "<br>" +
                red(bold(aux::unicode::
                what(e.what())));
            }
        }

        void on_key(str key, bool down, bool input) override
        {
            if (down
            and key == "enter"
            and focus.now == &Filename)
            {
                editing.stop();
                save();
            }
            else
            if (focus.now)
                focus.now->on_key(
                key, down, input);
        }
    };
}
