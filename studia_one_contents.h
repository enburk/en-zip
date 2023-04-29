#pragma once
#include "app_one.h"
namespace studio::one
{
    using std::filesystem::path;

    struct contents:
    widget<contents>
    {
        struct flist:
        widget<flist>
        {
            int selected = 0;
            array<int> indices;
            gui::widgetarium<gui::button> list;
            void on_change (void* w) override {
                if (w == &list) {
                selected = list.notifier_index;
                notify(); } }
        };

        flist flist;
        gui::scroller<gui::vertical> scroller;
        gui::binary_property<path> selected;
        gui::binary_property<path> root;

        gui::property<gui::time> timer;
        sys::directory_watcher watcher;
        std::atomic<bool> reload = true;

        struct record
        {
            path path;
            str  name;
            int  level = 0;
            bool open = false;
            bool file = false;
        };
        array<record> records;

        contents ()
        {
            watcher.dir = std::filesystem::current_path() / "content";
            watcher.action = [this](std::filesystem::path path, str what)
            {
                str e = path.extension().string();
                if (std::filesystem::is_regular_file(path)
                and e != ".txt") return;
                reload = true;
            };
            watcher.watch();

            root = watcher.dir;
            fill(root);

            str open = sys::settings::load("studio::one::content::open", "");
            str path = sys::settings::load("studio::one::content::path", "");

            array<str> opens = open.split_by(";");
            for (auto& record: records) if (not record.file)
            record.open = opens.contains(record.path.string());
            selected = path.c_str();
        }

        void refresh ()
        {
            int t = scroller.top;
            int W = coord.now.w; if (W <= 0) return;
            int H = coord.now.h; if (H <= 0) return;
            int h = gui::metrics::text::height*12/10;
            int l = gui::metrics::line::width;
            int w = W;

            int hh = h * flist.list.size();
            if (hh <= H) w = W;

            flist     .coord = xyxy(0, 0, w, H);
            scroller  .coord = xyxy(w, 0, W, H);
            flist.list.coord = xywh(0, 0, w, hh);

            int y = 0;
            for (auto & line : flist.list) {
            line.coord = xywh(0, y, w, h);
            y += h; }

            scroller.span = hh;
            scroller.step = h;
            scroller.top  = t; xywh r =
            flist.list.coord; r.y = -scroller.top;
            flist.list.coord = r;
        }

        void replane ()
        {
            int n = 0;
            int index =-1;
            int level = 1;
            str list_of_opens;
            flist.indices.clear();
            for (auto& record: records)
            {
                if (record.open and not record.file)
                list_of_opens += record.path.string() + ";";

                index++;
                if (record.level > level)
                    continue;

                str name = record.name;
                if (name.starts_with("''")
                and name.  ends_with("''")) {
                    name.truncate(); name.erase(0);
                    name.truncate(); name.erase(0);
                    name = extracolor(
                    name); }

                str html;
                for (int i=0; i<record.level-1; i++) html += mspace;
                html += record.file ? mspace : record.open ? (char*)(u8"− ") : "+ ";
                html += record.file ? name : bold(name);

                flist.indices += index;
                auto& it = flist.list(n++);
                it.text.alignment = xy(pix::left, pix::center);
                it.kind = record.file ? gui::button::sticky : gui::button::normal;
                it.on = record.file and record.path == selected.now;
                it.text.html = html;

                if (not record.file)
                level = record.open ?
                        record.level+1:
                        record.level;
            }
            flist.list.truncate(n);
            sys::settings::save(
            "studio::one::content::open",
            list_of_opens);
        }

        void fill(path dir, int level = 1)
        {
            using namespace std::filesystem;

            std::map<str, path> paths;

            for (directory_iterator next(dir), end; next != end; ++next)
            {
                path p = next->path();
                if (is_directory (p))
                {
                    str name = p.filename().string();
                    if (name.starts_with(".")) continue;
                    paths[p.filename().string()] = p;
                }
                if (is_regular_file (p))
                {
                    auto ext = p.extension();
                    if (ext != ".txt") continue;
                    paths[p.stem().string()] = p;
                }
            }

            for (auto [name, path] : paths)
            {
                record r;
                r.path = path;
                r.name = name.from(3); str options;
                r.name.split_by(" # ", r.name, options);
                r.open = is_regular_file(path);
                r.file = is_regular_file(path);
                r.level = level;
                records += r;
                if (!r.file)
                fill(r.path, level+1);
            }
        }

        void on_change (void* what) override
        {
            if (timer.now == gui::time())
                timer.go(gui::time::infinity,
                         gui::time::infinity);

            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                refresh();
            }

            if (what == &skin)
            {
                replane();
                refresh();
            }

            if (what == &timer and reload)
            {
                reload = false;

                array<path> opens;
                for (auto& record: records)
                if (record.open) opens += record.path;
                records.clear();

                fill(root);

                bool found = false;
                for (auto& record: records) {
                if (opens.contains(record.path)) record.open = true;
                if (record.file and record.path == selected) found = true; }
                if (not found) selected = path{};

                replane();
                refresh();
                notify();
            }

            if (what == &flist)
            {
                int i = flist.selected;
                if (i >= 0 and i < flist.indices.size())
                {
                    i = flist.indices[i];
                    if (i >= 0 and i < records.size())
                    {
                        if (records[i].file) selected =
                            records[i].path; else {
                            records[i].open = not
                            records[i].open;
                            replane();
                            refresh();
                        }
                    }
                }
            }

            if (what == &selected)
            {
                sys::settings::save(
                "studio::one::content::path",
                selected.now.string());

                replane();
                notify();
            }

            if (what == &scroller) { xywh r =
                flist.list.coord; r.y = -scroller.top;
                flist.list.coord = r;
            }
        }

        bool on_mouse_wheel (xy p, int delta) override
        {
            int h = scroller.step;
            delta /= abs(delta) < 20 ? abs(delta) : 20;
            delta *= h > 0 ? h : gui::metrics::text::height;
            if (sys::keyboard::ctrl ) delta *= 5;
            if (sys::keyboard::shift) delta *= coord.now.h;
            int d = flist.coord.now.h - flist.list.coord.now.h; // may be negative
            int y = flist.list.coord.now.y + delta;
            if (y < d) y = d;
            if (y > 0) y = 0;
            scroller.top =-y;
            return true;
        }
    };
}
