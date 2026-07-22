#pragma once
#include "app.h"
#include "content_entry+.h"
namespace studio
{
    using Res = media::resource;
    using res = media::resource*;
    using Ent = content::out::entry;
    using ent = content::out::entry*;

    struct voc
    {
        array<ent> entries;
        array<res> resources;
    };

    auto all_entries(res r)
    {
        auto entries = r->entries;
        auto title = doc::html::untagged(r->title);

        if ((r->kind == "audio"
        and entries.size() == 0
        and not r->options.contains("=")
        and not r->options.contains("=="))
        or  entries.contains("+"))
            entries += eng::parser::entries(
            app::vocabulary, title,
            r->options.contains
            ("Case"));
        else
        if (title.contains("/"))
        for (str s: title.split_by("/"))
            entries += s;

        entries.try_erase("+");
        return entries;
    }

    str cliplink (res const& r)
    {
        str s = r->full();
        if (r->options.contains("sound"))
            s = "[" + s + "]";

        str abstract = r->abstract;
        if (r->options.contains("sound"))
            abstract += " # SOUND";

        return linked(
        dark(html(s)),
        "clipboard://: " + abstract +
        "file://" + str(r->path));
    }
    str cliplink (res r, hashset<str>& course_vocabulary)
    {
        array<str> missed;
        for (str x: all_entries(r))
        if  (x.size() >= 2 // skip "," "!" "?"
        and not x.contains(" ") // "see you"
        and not course_vocabulary.contains(
            eng::lowercased(simple(x))))
            missed += x;

        str s = r->full();
        if (r->options.contains("sound"))
            s = "[" + s + "]";

        str abstract = r->abstract;
        if (r->options.contains("sound"))
            abstract += " # SOUND";

        return linked(
        dark(html(s)) + red(" [" + str(missed, "] [") + "]"),
        "clipboard://: " + abstract +
        "file://" + str(r->path));
    }
    str link (media::resource const* r)
    {
        str s = r->full();
        if (r->options.contains("sound"))
            s = "[" + s + "]";

        return linked(
        dark(html(s)),
        "file://" + str(r->path));
    }
    str link (content::out::entry const* entry)
    {
        return linked(
        html(entry->eng) + "  " +
        dark(entry->pretty_link()),
        "one://" + entry->link);
    }
    str link (content::out::entry const& e)
    {
        return link(&e);
    }

    generator<str> wrong_words(content::out::entry const& entry)
    {
        if  (not entry.opt.internal.contains("sic!"))
        for (str sssss: entry.en*entry.us*entry.uk)
        for (str ssss: sssss.split_by("/"))
        for (str sss: ssss.split_by("|"))
        for (str ss: sss.split_by(" "))
        for (str s: ss.split_by("\\"))
        {
            s.erase_all('(');
            s.erase_all(',');
            s.erase_all('.');
            s.erase_all(':');
            s.erase_all(';');
            s.erase_all('?');
            s.erase_all('!');
            s.erase_all('"');
            s.erase_all(')');
            s.replace_all(u8"‘", "");
            s.replace_all(u8"’", "");
            if (s.ends_with("s'")) s.truncate();
            if (s.ends_with("'s")) s.truncate(), s.truncate();
            str w = eng::lowercased(s);
            str ww = eng::lowercased(ss);
            str www = eng::lowercased(sss);
            if (not app::vocabulary.contains(w)
            and not app::vocabulary.contains(s)
            and not app::vocabulary.contains(ww)
            and not app::vocabulary.contains(ss)
            and not app::vocabulary.contains(www)
            and not app::vocabulary.contains(sss))
            co_yield w;
        }
    }
}