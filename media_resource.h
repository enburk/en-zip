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
                str(path.filename()):
                str(path.stem());

            fn = un_msdos(fn);
            fn.strip();

            auto check = [path](str s) {
                if (s == "") return false;
                if (not s.contains("}}"))
                logs::err << red(bold(
                "no matching }}: " +
                str(path)));
                return true; };

            yadda = fn.extract_from("###");
            title = fn.extract_upto("{{"); bool meta_present = check(fn);
            meta  = fn.extract_upto("}}");

            str optio = title.extract_from("##");
            str links = title.extract_from("[" ); links.strip("[]");
            comment   = title.extract_from("%%");
            sense     = title.extract_from("@" );

            if (fn.starts_with("## "))
                optio += fn; else
                yadda += fn;

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

                if (credit == "")
                    credit = "-";
            }

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
                str(path)));

            abstract = title;
            abstract.replace_all("_", "");
            if (sense != "")
            abstract += "@" +
                sense;

            entries += links.split_strip_by("][");
            options += optio.split_strip_by("##");
            entries.erase_all("="); // voicebunny processing legacy
            entries.erase_all("");
            options.erase_all("");
        }

        generator<str> Entries ()
        {
            co_yield abstract;
            for (str e: entries)
            co_yield e;
        }

        str opt (str kind) const
        {
            for (str o: options)
            if (o.starts_with(kind + " "))
            return o.from(kind.size()+1);
            return "";
        }

        str full () const
        {
            str s = title;
            if (sense != "")
            s += " @ " + sense;
            return s;
        }

        str text () const
        {
            str s = title;
            if (sense != "") s += " @ " + sense;
            if (comment != "") s += " %% " + comment;
            if (not entries.empty()) s += " [" + str(entries, "][") + "]";
            if (not options.empty()) s += " ## " + str(options, " ## ");
            if (meta != "") s += " {{" + meta + "}}";
            if (yadda != "") s += " ### " + yadda;
            return to_msdos(s);
        }

        friend void operator >> (sys::in::pool& pool, resource& x) {
            str fn;
            pool >> fn;
            pool >> x.id;
            pool >> x.kind;
            pool >> x.title;
            pool >> x.abstract;
            pool >> x.sense;
            pool >> x.comment;
            pool >> x.credit;
            pool >> x.meta;
            pool >> x.yadda;
            pool >> x.options;
            pool >> x.entries;
            x.path = str2path(fn);
        }
        friend void operator << (sys::out::pool& pool, resource const& x) {
            pool << str(x.path);
            pool << x.id;
            pool << x.kind;
            pool << x.title;
            pool << x.abstract;
            pool << x.sense;
            pool << x.comment;
            pool << x.credit;
            pool << x.meta;
            pool << x.yadda;
            pool << x.options;
            pool << x.entries;
        }
    };
}