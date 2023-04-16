#pragma once
#include "media.h"
namespace media
{
    struct Resource
    {
        path path;
        str title;
        str comment, credit;
        array<str> options;
        array<str> entries;

        Resource () = default;

        Resource (std::filesystem::path path) : path(path)
        {
            bool title_stop = false;
            array<str> title_lines;
            array<str> comment_lines;
            for (str line: sys::in::text_lines(path))
            {
                line.strip();
                if (line.starts_with("###"))
                    break;

                if (not aux::unicode::ok(line))
                logs::err << red(bold(path.string() +
                " !UTF-8: [" + line + "]"));

                if (line.starts_with("**")) {
                    title_stop = true; str
                    comment = line.from(2);
                    comment.strip();
                    comment_lines +=
                    comment;
                }
                else
                if (line.starts_with("##")) {
                    title_stop = true;
                    str option = line.from(2); option.strip();
                    if (option == "") continue;
                    if (option.starts_with("credit ")) {
                        credit = option.from(7);
                        credit.strip();
                        continue;
                    }
                    options += option;
                }
                else
                if (line.starts_with("[")) { line.strip("[]");
                    entries += line.split_by("][");
                    title_stop = true;
                }
                else
                if (not title_stop)
                    title_lines +=
                    line;
            }

            while (title_lines.size() > 0
               and title_lines.back() == "")
                   title_lines.truncate();

            if (title_lines.size() > 0)
                title = str(title_lines,
                title_lines.front().
                starts_with("<") ?
                "" : "<br>");

            bool ok = title.rebracket("{","}", [&](str link)
            {
                link.erase(0);
                link.truncate();
                if (link != "")
                {
                    str S, s; bool capital = false;
                    for (str c: aux::unicode::glyphs(link))
                    {
                        if (c == "_") { capital = true; continue; }
                        S += c; if (not capital) c = eng::lowercased(c);
                        s += c; capital = false;
                    }
                    entries += S; if (s != S)
                    entries += s;
                }
                return link;
            });

            if (not ok)
            logs::err << red(bold(
            "parse links error: " +
                path.string()));

            if (comment_lines.size() > 0)
                comment = str(
                comment_lines,
                "<br>");
        }

        str opt (str kind) const
        {
            for (str o: options)
            if (o.starts_with(kind + " "))
            return o.from(kind.size()+1);
            return "";
        }
        void opt (str kind, str value)
        {
            for (str& o: options)
            if (o.starts_with(kind + " "))
            o = kind + " " + value;
        }
    };
}