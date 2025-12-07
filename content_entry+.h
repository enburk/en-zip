#pragma once
#include "app.h"
#include "content_entry.h"
#include "media_resource.h"

bool vocal_fits (content::out::entry& ent, media::resource& res)
{
    str a = ent.abstract;
    str r = res.abstract;

    if (ent.en.size() == 1
    and ent.uk.size() == 0
    and ent.us.size() == 0)
    for (str x: ent.en) if (x.contains("|"))
    for (str s: x.split_strip_by("|")) if (s == r)
        return true;

    if (r == a)
        return true;
    if (r == a + "@@")
        return true;
    if (r.contains("@"))
        return false;
    if (r == a.upto_first("@"))
        return true;

    if (r.starts_with("a "  )) r = str(r.from(2)) + "@noun"; else
    if (r.starts_with("an " )) r = str(r.from(3)) + "@noun"; else
    if (r.starts_with("the ")) r = str(r.from(4)) + "@noun"; else
    if (r.starts_with("to " )) r = str(r.from(3)) + "@verb"; else
        return false;

    if (r == a)
        return true;

    a = a.upto_first("@");

    if (not ent.noun and not ent.verb and not ent.adxx)
    {
        auto index = app::vocabulary.index(a);
        if (!index)
            return false;

        auto entry = app::dictionary.load(*index);
        for (auto topic: entry.topics)
        ent.noun |= topic.header == "noun",
        ent.noun |= topic.header == "proper noun",
        ent.verb |= topic.header == "verb",
        ent.adxx |= topic.header == "adjective",
        ent.adxx |= topic.header == "adverb";
    }

    if (ent.the_noun()
    and r == a + "@noun")
        return true;
            
    if (ent.the_verb()
    and r == a + "@verb")
        return true;

    return false;
}

bool vocal_will_not_fit (content::out::entry& ent, media::resource& res)
{
    int parts = 0;
    if (ent.mark_noun) parts++;
    if (ent.mark_verb) parts++;
    if (ent.mark_adxx) parts++;

    if (parts > 1)
    if (res.abstract.starts_with("a "  )
    or  res.abstract.starts_with("an " )
    or  res.abstract.starts_with("the ")
    or  res.abstract.starts_with("to " ))
    return true;
    return false;
}

bool video_fits (content::out::entry& ent, media::resource& res)
{
    array<str> fits = res.Entries();
    for (str s: array<str>(fits))
    if (s.starts_with("a "  )) fits += str(s.from(2)); else
    if (s.starts_with("an " )) fits += str(s.from(3)); else
    if (s.starts_with("the ")) fits += str(s.from(4)); else
    if (s.starts_with("to " )) fits += str(s.from(3)); else
    {}

    if (ent.sense == ""
    or  ent.sense == "@")
    for (str& s: fits)
    s = s.extract_upto("@");

    for (str s: fits)
    if (ent.video_matches.contains(s))
    return true;
    return false;
}

bool sound_fits (content::out::entry& ent, media::resource& res)
{
    return video_fits(ent, res);
}

