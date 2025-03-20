#pragma once
#include <set>
#include <regex>
#include "eng_abc.h"
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
        if (s.ends_with(kind))
        {
            s.resize(s.size() -
                kind.size());
        }
        return s;
    }

    auto forms (str s)
    {
        std::set<str> forms;
        std::set<str> backforms;
        forms.emplace(s);

        const hashset<str> excepts = 
        {
            "me", "so", 
        };

        if (excepts.contains(s))
            return forms;
        
        const array<str> kinds =
        {
            "ness", "less", 
            "ibility", "ability",
            "tion", "sion", "ful",
            "able", "ably", "ible", "ibly", 
            "ist", "ism", "ian", "ly", "al",
            "ing", "ed", "s",
        };

        for (str kind: kinds)
        {
            auto
            ff = backforms;
            ff.emplace(s);
            for (str f: ff)
            {
                str b = backform(f, kind);
                if (b != s) backforms.
                emplace(b);
            }
        }

        forms = backforms;
        forms.emplace(s);

        for (str kind: kinds)
        {
            if (backforms.empty()) forms.emplace(form(s, kind)); else
            for (str b: backforms) forms.emplace(form(b, kind));
        }

        return forms;
    }
}
