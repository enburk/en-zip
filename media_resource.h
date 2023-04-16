#pragma once
#include "media.h"
namespace media
{
    struct resource
    {
        path path;
        str id, kind;
        str title, abstract;
        str sense, comment, credit;
        str meta, yadda;
        array<str> options;
        array<str> entries;
        int usage[3] = { 0,  0,  0};
        int index[3] = {-1, -1, -1};
        int weight = max<int>();

        resource () = default;

        resource (std::filesystem::path path) : path(path)
        {
            if (not exists(path)) return;

            str fn = is_directory(path) ?
                path.filename().string():
                path.stem().string();

            fn = un_msdos(fn);
            fn.strip();

            auto check = [path](str name) {
                if (name == "") return false;
                if (not name.contains("}}"))
                logs::err << red(bold(
                "no matching }}: " +
                 path.string()));
                return true; };

            yadda = fn.extract_from("###");
            title = fn.extract_upto("{{"); bool meta_present = check(fn);
            meta  = fn.extract_upto("}}");
            yadda = fn;

            if (meta_present) // even if empty
            {
                str license;
                meta.split_by("$", credit, license);
                license.strip();
                credit.strip();
                
                if (credit.starts_with("NASA") or credit.starts_with("ESA") or
                    credit.starts_with("NOAA") or credit.starts_with("ESO") or
                    credit.starts_with("NNSA") or credit.starts_with("USN") or
                    credit.starts_with("USDA") or credit.starts_with("DOE") or
                    credit.starts_with("USDE") or credit.starts_with("NIH") or
                    credit.starts_with("USGS") or
                    credit.starts_with("USAF"))
                    credit = "Credit: " +
                    credit;

                if (license.starts_with("dreamstime")) credit += "/Dreamstime";
                if (license.starts_with("wiki"      )) credit += "/Wikimedia";
                if (license.starts_with("cc-by-sa"  )) credit += " CC-BY-SA"; else
                if (license.starts_with("cc-by"     )) credit += " CC-BY";
            }

            str optio = title.extract_from("##");
            str links = title.extract_from("[" ); links.strip("[]");
            comment   = title.extract_from("%%");
            sense     = title.extract_from("@" );

            if (yadda.starts_with("## "))
                optio += yadda;

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
                    if (links != "")
                    links += "][";
                    links += S;
                    if (s != S)
                    links += "][",
                    links += s;
                }
                return link;
            });

            if (not ok)
            logs::err << red(bold(
            "parse links error: " +
                path.string()));

            abstract = title;
            entries += links.split_strip_by("][");
            options += optio.split_strip_by("##");
            entries.erase_all("");
            options.erase_all("");

            // voicebunny processing legacy
            std::erase_if(entries, [](auto s)
                { return s == "="; });
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