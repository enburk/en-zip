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

        using text = app::dic::html_view;
        using edln = gui::text::one_line_editor;
        using edit = gui::text::editor;
        using Edln = gui::area<edln>;
        using Edit = gui::area<edit>;

        text filepath;
        Edln Filename; edln& filename = Filename.object;
        Edit Textfile; edit& textfile = Textfile.object;

        str filename_text;
        str textfile_text;

        gui::canvas canvas;
        gui::radio::group crops;
        gui::button button_play;
        gui::button button_stop;
        gui::button button_Zoom;
        gui::button button_zoom;
        gui::button button_full;
        gui::button button_crop;
        gui::button button_qrop;
        sfx::media::player full;
        sfx::media::player crop;
        sfx::media::player qrop;
        gui::button button_delete;
        gui::button button_revert;

        gui::timer loading;
        gui::timer editing;

        int zoom = 100;
        str ZOOM = "studio::dic::detail::zoom";
        int clicked = 0;

        detail () { select(""); }
       ~detail () { select(""); }

        void save () try
        {
            str
            text = textfile.text;text.trimr();
            if (textfile_text != text) {
                textfile_text  = text;

                if (textfile_text == ""
                and exists(txtpath))
                    remove(txtpath);
                else
                sys::out::write(txtpath,
                textfile_text.lines());
                // crop can be changed
                select(datpath.string());
            }
            text = filename.text;text.strip();
            if (filename_text != text) {
                filename_text  = text;

                if (filename_text == "")
                    return;

                path dir = datpath.parent_path();
                path datpath_ = dir / (filename_text + ".dat");
                path txtpath_ = dir / (filename_text + ".txt");
                datpath_.replace_extension(datpath.extension());
                rename(datpath, datpath_); if (exists(txtpath))
                rename(txtpath, txtpath_);
                // crop can be changed
                select(datpath_.string());
            }
        }
        catch (std::exception const& e)
        {
            filepath.html = str(
            filepath.html) + "<br>" +
            red(bold(aux::unicode::
            what(e.what())));
        }

        void select (str info)
        {
            save();

            button_play.show();
            button_stop.hide();
            button_play.enabled = false;
            button_stop.enabled = false;
            button_full.enabled = false;
            button_crop.enabled = false;
            button_qrop.enabled = false;

            full.load({},{});
            crop.load({},{});
            qrop.load({},{});

            datpath = info.c_str();
            txtpath = info.c_str();
            txtpath.replace_extension(".txt");

            str s = info;
            s.replace_all("\\", "/");
            if (s.starts_with("../"   )) s = s.from(3);
            if (s.starts_with("datae/")) s = s.from(6);
            
            str tree = "";
            for (str x: s.split_by("/"))
            tree += gray(">> ") + x + "<br>";

            filepath.html = tree;
            filename_text = datpath.stem().string();
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

            auto load = [](sfx::media::player& player, path source, str crop, str fade)
            {
                str stem = source.stem().string();
                str ext = source.extension().string();

                if (crop != "") stem += " == crop " + crop;
                if (fade != "") stem += " == fade " + fade;

                path
                cache = "../data/!detail";
                cache /= stem + ext;

                ext = ext.ascii_lowercased();
                if (media::audioexts.contains(ext))
                cache.replace_extension(".ogg");

                array<byte> video_bytes;
                array<byte> audio_bytes;
                if (media::videoexts.contains(ext))
                video_bytes = media::video::readsample(source, cache, crop).value(); else
                audio_bytes = media::audio::readsample(source, cache, crop, fade).value();

                player.load(
                std::move(video_bytes),
                std::move(audio_bytes));
            };

            str text =
            textfile.text != "" ?
            textfile.text:
            filename.text;

            text.replace_all("\n", " ");
            str yadda= text.extract_from("{{");
            array<str> options = text.split_strip_by(" ## ");
            options.upto(1).erase();

            str ocrop, oqrop, ofade;
            for (str o: options) {
            if (o.starts_with("crop ")) ocrop = o.from(5);
            if (o.starts_with("qrop ")) oqrop = o.from(5);
            if (o.starts_with("fade ")) ofade = o.from(5);
            }

            bool cropped = ocrop != "" or ofade != "";
            bool qropped = oqrop != "";

            load(full, datpath, "",    ""   ); if (cropped)
            load(crop, datpath, ocrop, ofade); if (qropped)
            load(qrop, datpath, oqrop, ofade);

            button_play.enabled = true;
            button_stop.enabled = true;
            button_full.enabled = true;
            button_crop.enabled = cropped;
            button_qrop.enabled = qropped;
            button_full.on = true;

            loading.start();
        }

        void refresh ()
        {
            auto resize = [this](sfx::media::player& player)
            {
                int y = 
                button_play.coord.now.y +
                button_play.coord.now.h;
                int x = 0;

                xy size = player.resolution;
                int maxsizex = coord.now.w;
                int maxsizey = coord.now.h - y;

                int sizex = maxsizex * zoom / 100;
                int sizey = maxsizey * zoom / 100;

                double k = sqrt ((double)(sizex*sizey) / (size.x*size.y));

                double kx = (double) size.x / sizex;
                double ky = (double) size.y / sizey;

                double fsizex = size.x * k;  int isizex = int (fsizex + 0.5);
                double fsizey = size.y * k;  int isizey = int (fsizey + 0.5);

                if (isizex > sizex
                or  isizey > sizey) // downscale
                {
                    if (kx > ky) { size = sizex * size / size.x; k = 1/kx; }
                    else         { size = sizey * size / size.y; k = 1/ky; }
                }

                x += (maxsizex - size.x) / 2;
                y += (maxsizey - size.y) / 2;

                player.coord = xywh(x, y,
                size.x, size.y);
            };
            resize(full);
            resize(crop);
            resize(qrop);
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

                button_full.coord = xywh(0*W/6, y, W/6, h);
                button_crop.coord = xywh(1*W/6, y, W/6, h);
                button_qrop.coord = xywh(2*W/6, y, W/6, h);
                button_Zoom.coord = xywh(3*W/6, y, W/6, h);
                button_zoom.coord = xywh(4*W/6, y, W/6, h);
                button_play.coord = xywh(5*W/6, y, W/6, h);
                button_stop.coord = xywh(5*W/6, y, W/6, h);
                y += h;

                canvas.coord = xyxy(0, y, W, H);

                button_delete.coord = xywh(0, H-h, W/6, h);
                button_revert.coord = xywh(0, H-h, W/6, h);

                refresh();
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

                canvas.color = s.ultralight.first;

                button_full.kind = gui::button::sticky;
                button_crop.kind = gui::button::sticky;
                button_qrop.kind = gui::button::sticky;

                button_play.text.text = "play";
                button_stop.text.text = "stop";
                button_Zoom.text.text = "zoom+";
                button_zoom.text.text = "zoom-";
                button_full.text.text = "full";
                button_crop.text.text = "crop";
                button_qrop.text.text = "qrop";

                button_delete.text.html = red("delete");
                button_revert.text.html = red("revert");

                zoom = sys::settings::load(
                ZOOM, 100);
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

            if (what == &loading
            and full.status != sfx::media::state::loading
            and crop.status != sfx::media::state::loading
            and qrop.status != sfx::media::state::loading)
            {
                bool video =
                media::videoexts.contains(
                datpath.extension().string());
                if (video and button_full.on) full.play();
                if (video and button_crop.on) crop.play();
                if (video and button_qrop.on) qrop.play();
                if (video) button_play.hide();
                if (video) button_stop.show();
                loading.stop();
                refresh();
            }
            if (what == &button_Zoom)
            {
                zoom = min(100, zoom+10);
                sys::settings::save(ZOOM, zoom);
                refresh();
            }
            if (what == &button_zoom)
            {
                zoom = max(10, zoom-10);
                sys::settings::save(ZOOM, zoom);
                refresh();
            }
            if (what == &button_play)
            {
                if (button_full.on) full.play();
                if (button_crop.on) crop.play();
                if (button_qrop.on) qrop.play();
                button_play.hide();
                button_stop.show();
            }
            if (what == &button_stop)
            {
                if (button_full.on) full.stop();
                if (button_crop.on) crop.stop();
                if (button_qrop.on) qrop.stop();
                button_play.show();
                button_stop.hide();
            }
            if (what == &button_full)
            {
                button_crop.on = false;
                button_qrop.on = false;
                full.show();
                crop.hide();
                qrop.hide();
                full.play();
            }
            if (what == &button_crop)
            {
                button_full.on = false;
                button_qrop.on = false;
                full.hide();
                crop.show();
                qrop.hide();
                crop.play();
            }
            if (what == &button_qrop)
            {
                button_full.on = false;
                button_crop.on = false;
                full.hide();
                crop.hide();
                qrop.show();
                qrop.play();
            }
            if (what == &button_delete) try
            {
                path dir = datpath.parent_path()/".del";
                std::filesystem::create_directories(dir);
                path datpath_ = dir / datpath.filename();
                path txtpath_ = dir / txtpath.filename();
                rename(datpath, datpath_); if (exists(txtpath))
                rename(txtpath, txtpath_);
                select(datpath.string());
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
                select(datpath.string());
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
            if (key == "enter" && down)
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
