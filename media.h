#include "eng.h"
namespace media
{
    using std::byte;
    using std::filesystem::path;

    struct resource
    {
        path path;

        str entry;
        str sense;
        str title;
        str comment;
        str license;
        str credit;

        array<str> entries;
        array<str> options;
        array<str> keywords;

        bool used = false;
    };

    array<resource> scan (path dir)
    {
        array<resource> resources;
        for (std::filesystem::directory_iterator
            next(dir), end; next != end; ++next)
        {
            path p = next->path();
            str name = p.filename().string();

            if (is_directory(p))
            {
                if (name.starts_with(".")) continue;

                resources += scan(p);
            }
            if (is_regular_file(p))
            {
                //auto ext = p.extension();
                //if (ext != ".ae!" && ext != ".ae"
                //&&  ext != ".cpp" && ext != ".hpp"
                //&&  ext != ".cxx" && ext != ".hxx"
                //&&  ext != ".c++" && ext != ".h++" && ext != ".h") continue;
                //paths += p;
            }
        }

        //paths.sort([](path a, path b) { return less(a,b); });
        //
        //for (auto p : paths) {
        //    if (is_directory (p)) {
        //        auto & it = subs.emplace_back();
        //        it.text.alignment = XY(gui::text::left, gui::text::center);
        //        it.text.text = std::filesystem::relative(p, root.now).string();
        //        it.enabled = false;
        //    }
        //    if (is_regular_file (p)) {
        //        auto & it = subs.emplace_back();
        //        it.text.alignment = XY(gui::text::left, gui::text::center);
        //        it.text.text = std::filesystem::relative(p, root.now).string();
        //    }
        //}
        //dir.text = root.now.string();
    }
}