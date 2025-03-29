#pragma once
#include <set>
#include <regex>
#include "eng_abc_vocabulary.h"
namespace eng::list
{
    array<str> contractions = str(
    "I'm, I'll, "
    "you're, you'll"
    "he's, he'll"
    "she's, she'll"
    "it's, it'll"
    "we're, we'll"
    "that's, that'll"
    "aren't, doesn't, don't"
    "can't, couldn't, shouldn't, wouldn't"
    "ain't"
    ).split_by(", ");

    array<str> contractionparts = str(
    "don, m, s, re, ll, t").split_by(", ");

    array<str> sensitive = str(
    "aggravated, angry, annoyed, anxious, astonished, "
    "bewildered, concerned, "
    "defiant, derogatory, dejected, depressed, desperate, "
    "disappointed, disbelieving, disinterested, "
    "emphatic, enthusiastic, exaggerated, excited, exuberant, "
    "forlorn, frustrated, gratified, happy, horrified, "
    "impatient, incredulous, indignant, "
    "playfully, realization, relieved, "
    "sad, sarcastic, scared, shocked, stern, suprised, sympathetic, "
    "unimpressed, upset, urgent"
    ).split_by(", ");

}
namespace eng
{
    const str vowels = u8"aeiouáéíóúàèìòùâêîôûäëïöüæœø";
	const str VOWELS = u8"AEIOUÁÉÍÓÚÀÈÌÒÙÂÊÎÔÛÄËÏÖÜÆŒØ";
    const str Vowels = vowels + VOWELS;

    bool last_cons (str s) {
	    const std::regex r1("^[Yy][" + vowels + "y]([^A-Z" + vowels + "ywxh])$");
		const std::regex r2("^[^" + Vowels + "yY]*[" + 
            Vowels + "yY]([^A-Z" + vowels + "ywxh])$");
        return std::regex_match(s, r1) or
               std::regex_match(s, r2); }

    str form (str s, str kind)
    {
        if (kind == "s")
        {
            if (s.ends_with("s" ) or
                s.ends_with("ss") or
                s.ends_with("sh") or
                s.ends_with("ch") or
                s.ends_with("x" ) or
                s.ends_with("z" ))
                s += "es";
            else
            if (s.ends_with("ay") or
                s.ends_with("ey") or
                s.ends_with("iy") or
                s.ends_with("oy") or
                s.ends_with("uy"))
                s += "s";
            else
            if (s.ends_with("y")) {
                s.truncate();
                s += "ies"; }
            else
                s = s + "s";
        }
        else
        if (kind == "ing")
        {
            if (last_cons(s)) s += s.back();
            else
            if (std::regex_match(s, std::regex("^(.*)ie$"))) {
                s.truncate(); s.truncate(); s += "y";
            }
            else
            if (std::regex_match(s, std::regex("^(.*)ue$")) or
                std::regex_match(s, std::regex("^(.*[" +
                Vowels + "yY][^" + vowels + "y]+)e$"))) {
                s.truncate();
            }
            s += "ing";
        }
        else
        if (kind == "ed")
        {
            if (last_cons(s)) s += s.back(); else
            if (s.ends_with("ay")) { ; } else
            if (s.ends_with("ey")) { ; } else
            if (s.ends_with("iy")) { ; } else
            if (s.ends_with("oy")) { ; } else
            if (s.ends_with("uy")) { ; } else
            if (s.ends_with("y" )) { s.truncate(); s += "i"; } else
            if (s.ends_with("e" )) { s.truncate(); }
            s += "ed";
        }
        else
        if (kind == "al"
        or  kind == "ly"
        or  kind == "ness")
        {
            if (s.ends_with("y" )) { s.truncate(); s += "i"; }
            s += kind;
        }
        else
        if (kind == "tion")
        {
            if (s.ends_with("t" )) s.truncate();
            s += kind;
        }
        else s += kind;
        return s;
    }

    str backform (str s, str kind)
    {
        if (s.size() -
         kind.size() < 3)
        {
            // do nothing
        }
        else
        if (kind == "s")
        {
            if (s.ends_with("ies"))
                s.truncate(),
                s.truncate(),
                s.truncate(),
                s += "y";
            else
            if (s.ends_with("es"))
                s.truncate(),
                s.truncate();
            else
            if (s.ends_with("s"))
                s.truncate();
        }
        else
        if (kind == "ing" and s.ends_with("ing"))
        {
            s.truncate();
            s.truncate();
            s.truncate(); if (last_cons(s))
            s.truncate();
        }
        else
        if (kind == "ed" and s.ends_with("ed"))
        {
            s.truncate();
            s.truncate(); if (last_cons(s))
            s.truncate();
        }
        else
        if (kind == "ness" and s.ends_with("ness"))
        {
            s.truncate();
            s.truncate();
            s.truncate();
            s.truncate(); if (s.ends_with("i"))
            s.truncate(), s += "y";
        }
        else
        if (kind == "ly" and s.ends_with("ly"))
        {
            s.truncate();
            s.truncate(); if (s.ends_with("i"))
            s.truncate(), s += "y";
        }
        else
        if (kind == "ic" and s.ends_with("ic"))
        {
            s.truncate();
            s.truncate();
            s += "y";
        }
        else
        if (s.ends_with(kind))
        {
            s.resize(s.size() -
                kind.size());
        }
        return s;
    }

    str backform2 (str s, str kind)
    {
        if (s.size() -
         kind.size() < 3)
        {
            // do nothing
        }
        else
        if (kind == "s" and s.ends_with("es"))
        {
            s.truncate();
        }
        else
        if (kind == "ed" and s.ends_with("ed"))
        {
            s.truncate();
        }
        else
        if (kind == "ing" and s.ends_with("ing"))
        {
            s.truncate();
            s.truncate();
            s.truncate();
            s += "e";
        }
        else
        if (kind == "ly" and s.ends_with("ly"))
        {
            s.truncate();
            s.truncate();
            s += "e";
        }
        else
        if (kind == "ic" and s.ends_with("ic"))
        {
            s.truncate();
            s.truncate();
        }
        return s;
    }

    const array<str> combinable_suffixes =
    {
        "ology",
        "ness", "less",
        "tion", "sion", "ment",
        "able", "ible", "ably", "ibly", 
        "ist", "ism", "ian", "ish", "ize", "ise", "ful",
        "ly", "ic", "al", "y",
        "ing", "ed", "s",
    };

    auto upforms (str s, vocabulary const& vocabulary) -> array<str>
    {
        array<str> forms;
        if (not vocabulary.contains(s)) return forms;
        for (str suffix: combinable_suffixes)
        forms += upforms(form(s, suffix), vocabulary);
        forms += s;
        return forms;
    }

    auto backforms (str s) -> array<str>
    {
        array<str> forms; forms += s;
        for (str suffix: combinable_suffixes)
        {
            str f = backform(s, suffix);
            if (f != s) forms += backforms(f);
            str g = backform2(s, suffix);
            if (g != s) forms += backforms(g);
        }
        return forms;
    }

    auto combiforms (str s, vocabulary const& vocabulary)
    {
        array<str> forms;
        array<str> ff = backforms(s);
        for (str f: reverse(ff))
        if (not forms.contains(f))
        forms += upforms(f, vocabulary);
        return forms;
    }

    auto forms (str s, vocabulary const& vocabulary)
    {
        hashmap<str, array<str>> cache;

        array<str> & forms = cache[s];

        if (not forms.empty()) return forms;

        forms += s;

        const hashset<str> excepts = 
        {
            "me", "so", 
        };
        if (excepts.contains(s))
        return forms;

        forms += combiforms(s, vocabulary);
        forms.deduplicate();
        return forms;
    }
}
