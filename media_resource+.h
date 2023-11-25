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
            if (not exists(path)) return;

            bool title_stop = false;
            array<str> title_lines;
            array<str> comment_lines;
            for (str line: sys::text_lines(path))
            {
                line.strip();
                if (line.starts_with("###"))
                    break;

                if (not aux::unicode::ok(line))
                logs::err << red(bold(str(path) +
                " !UTF-8: [" + line + "]"));

                if (line.starts_with("%%")
                or  line.starts_with("**")) {
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
                    if (option.starts_with("crop ")
                    or  option.starts_with("qrop ")
                    or  option.starts_with("fade")) {
                        logs::err << red(bold(
                        "external only option " +
                         option + " in .txt: " +
                         str(path)));
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
                str(path)));

            if (comment_lines.size() > 0)
                comment = str(
                comment_lines,
                "<br>");
        }
    };
}