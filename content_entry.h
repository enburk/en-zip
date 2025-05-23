﻿#pragma once
#include "eng_parser.h"
#include "eng_phenomena.h"
#include "content_options.h"
namespace content
{
    array<str> Eng_markers = str(
    "{Am.}, {Br.}, {also}, {esp.}, {mainly}, {usually}, {informal}, {informal:}, "
    "{plural:}, {comparative:}, {superlative:}, {derived term:}, "
    ).split_by(", ");

    array<str> eng_markers = str("Am., Br., "
    "also, especially, mainly, usually, rarely, "
    "colloquial, informal, formal, idiomatic, idiom, "
    "literary, literally, old-fashioned, slang, vulgar, "
    "singular, plural, uncountable, countable, comparative, superlative, "
    "intransitive, transitive, past tense, past participle"
    ).split_by(", ");

    array<str> Rus_markers = str(
    u8"дословно:, иногда:, книжное:, разговорное:, "
    u8"редко:, реже:, сленг:, также:, устаревшее:, не только, " 
    u8"в математике:, в геометрии:, в физике:, в астрономии:, в биологии:, в зоологии:, в ботанике:, "
    u8"в философии:, в психологии:, в экономике:, в истории:, в лингвистике:, в фонетике:, в грамматике:, "
    u8"в спорте:, в футболе:, в хоккее:, в теннисе:, в гимнастике:, в гонках:, в плавании:, в шахматах:, в картах:, "
    u8"в искусстве:, в литературе:, в музыке:, в театре:, в архитектуре:, в Древнем Риме:, "
    u8"в политике:, в обиходе:, в медицине:, в хирургии:, в физиологии:, "
    u8"в авиации:, в морском деле:, в цифр. технологиях:"
    ).split_by(", ");

    array<str> rus_markers = str(
    u8"англ., исп., итал., лат., нем., порт., фр., "
    u8"в брит., в амер., в знач., и проч., истор., "
    u8"буквально:, дословно:, дословно c , например:, "
    u8"обычно:, сравните:, редко:, также:, формальное:"
    ).split_by(", ");
}
namespace content::out
{
    struct entry
    {
        str eng;
        str rus;
        str sense;
        str comment;
        str anomaly;
        options opt;
        array<str> en;
        array<str> uk;
        array<str> us;
        array<str> matches;
        array<str> video_matches;
        array<str> vocabulary;
        array<str> phrases;
        array<str> errors;
        str abstract;
        int line = 0;
        str link;

        entry () = default;
        entry (str s, path path, int line) :  line{line}
        {
            link = str(path) + "|" +
            str(line).right_aligned(2);

            s.strip();
            s.replace_all("\t", " ");

            eng = s;

            if (s == ""
            or  s.starts_with("~~~")
            or  s.starts_with("---")
            or  s.starts_with("***")
            or  s.starts_with("==="))
                return;

            comment = s.extract_from("///");

            opt = s.extract_from("#");
            rus = s.extract_from("=");
            eng = s;

            for (str o: opt.unknown)
            errors += "UNKNOWN OPTION: " + o;

            str
            commt = s.extract_from("%%");
            sense = s.extract_from("@");

            if (sense.contains("{")
            or  sense.contains("}"))
            errors += "SENSE: " + sense;

            s.replace_all("'''", "");
            s.replace_all("''" , "");
            s.replace_all("~",   "");
            s.canonicalize();
            s = simple(s);

            if (s == "") return;

            {
                str x = s;
                x.replace_all("|","");
                x.replace_all("/","");
                x.replace_all("\\","");
                x.replace_all("(1)","");
                x.replace_all("(2)","");
                x.replace_all("(3)","");
                x.replace_all("(4)","");
                x.replace_all("{Br.}","");
                x.replace_all("{Am.}","");
                if (x.contains(one_of ("{}()")) or
                commt.contains(one_of ("{}")))
                anomaly = "{}()[]";
            }

            parse(s);

            abstract = str(en*uk*us, ", ");
            abstract.replace_all("|",", ");

            for (str x: en*uk*us)
            matches += x.split_strip_by("|");
            matches += abstract;
            matches.deduplicate();

            for (str x: en*uk*us)
            video_matches += x.split_strip_by("|");
            video_matches += abstract;

            if (sense == "" and
            not s.starts_with(": ")
            and s.contains(str(u8" → ")))
            video_matches.upto(1).erase();
            video_matches.deduplicate();

            if (sense != "@")
            if (sense != "") for (str& x: video_matches)
            if (sense != x) x += "@" + sense;

            if (sense != "")
            abstract  += "@" +
                sense;
        }

        bool audio_fits (str r_abstract)
        {
            array<str> fits;
            const str& s = r_abstract; fits += s;
            if (s.starts_with("a "  )) fits += str(s.from(2)) + "@noun"; else
            if (s.starts_with("an " )) fits += str(s.from(3)) + "@noun"; else
            if (s.starts_with("the ")) fits += str(s.from(4)) + "@noun"; else
            if (s.starts_with("to " )) fits += str(s.from(3)) + "@verb"; else
            {}

            str a = abstract;
            if (not s.contains("@"))
            a = a.extract_upto("@");

            return fits.contains(a);
        }

        bool video_fits (array<str> video_entries)
        {
            auto fits = video_entries;
            for (str s: video_entries)
            if (s.starts_with("a "  )) fits += str(s.from(2)); else
            if (s.starts_with("an " )) fits += str(s.from(3)); else
            if (s.starts_with("the ")) fits += str(s.from(4)); else
            if (s.starts_with("to " )) fits += str(s.from(3)); else
            {}

            if (sense == ""
            or  sense == "@")
            for (str& s: fits)
            s = s.extract_upto("@");

            for (str s: fits)
            if (video_matches.contains(s))
            return true;
            return false;
        }

        void parse (str s)
        {
            if (s.starts_with(": "))
            {
                s = s.from(2);
                int spaces = 0;
                for (char c: s) if (c == ' ') spaces++;
                if (spaces < 5)
                {
                    str x = s;
                    x.replace_all(".", "");
                    x.replace_all("!", "");
                    x.replace_all("?", "");
                    x = s.ascii_lowercased();
                    phrases += x;
                }
                en += s;
                return;
            }

            if (s.contains(str(u8" → ")))
            {
                // might not → mightn’t ~ {esp.} {Br.}
                // spill → ~ spilled\spilt, ~ spilled\spilt
                if (s.contains("//")
                or  s.contains("\\\\"))
                errors += u8"complex → ";
                s.replace_all(u8" → ","/");
                s.replace_all(", ","/");
                s.replace_all("\\","/");
                parse(s);
                return;
            }

            if (s.contains("//"))
            {
                for (str x: s.split_strip_by("//"))
                parse(x);
                return;
            }

            if (s.contains("\\\\"))
            {
                str k =
                s.extract_upto("\\\\");
                if (not k.contains("{Br.}")) k += " {Br.}";
                if (not s.contains("{Am.}")) s += " {Am.}";
                parse(k);
                parse(s);
                return;
            }

            bool br = s.contains("{Br.}");
            bool am = s.contains("{Am.}");

            if (br and
                not us.empty())
                errors += "us before uk";

            if (not br and not am)
            if (not uk.empty() or
                not us.empty())
                errors += "en after uk or us";

            array<str> & audio =  br ? uk : am ? us : en;

            for (str marker: Eng_markers)
            s.replace_all(marker, "");
            s.replace_all("[", "");
            s.replace_all("]", "");
            s.replace_all("~", "");
            s.replace_all("||","|");
            s.debracket("(",")");

            if (s.contains(
            one_of   ("{_}")))
            errors += "{_}";

            for (str x: s.split_by("/"))
            {
                x.strip();
                x.replace_all("\\","/");
                x.replace_all("| ","|");
                x.replace_all(" |","|");
                
                phrases += x;
                if (opt.internal.contains("+ru"))
                x += " -- " + simple(rus);
                audio += x;
            }
        }

        void vocabulate (eng::vocabulary const& voc)
        {
            for (str p: phrases)
            for (str e: eng::parser::entries(voc, p, true))
            for (str f: eng::forms(e, voc))
            vocabulary += f;
            vocabulary.deduplicate();
        }

        str formatted (int tab1, int tab2) const
        {
            str E = eng;
            str R = rus;
            str O = opt.formatted();
            str C = comment;
            // eng... = rus... # opt
            // eng... = rus
            // eng... # opt
            // eng
            // = rus # opt
            // = rus
            // # opt
            // 
            int e = aux::unicode::length(eng);
            int r = aux::unicode::length(rus);
            int o = O == "" and C == "" ? 0 : 1;
            if (e and (r or  o)) E.align_left(tab1);
            if (e and (r and o)) R.align_left(tab2 - tab1 - 3);
            str s;
            if (E != "" ) s +=          E + " ";
            if (R != "" ) s +=   "= " + R + " ";
            if (O != "" ) s +=   "# " + O + " ";
            if (C != "" ) s += "/// " + C;
            s.strip();
            return s;
        }

        static
        str pretty_link (str link)
        {
            str line = link.extract_from("|");

            link.replace_all("\\", "/");
            link.resize(link.size()-4); // .txt
            array<str> ss = link.split_by("/");
            ss.upto(1).erase(); // content/
            for (str& s: ss) {
            s = s.from(3);
            if (s.starts_with("''")
            and s.  ends_with("''")) {
                s.truncate(); s.erase(0);
                s.truncate(); s.erase(0);
                s = extracolor(
                s); }
            }
            link = str(ss, blue("/"));
            return link;
        }
        str pretty_link () const { return
            pretty_link(link); }

        friend void operator << (sys::out::pool& pool, entry const& x) {
            pool << x.eng;
            pool << x.rus;
            pool << x.opt;
        }
    };
}
namespace content::in
{
    struct entry
    {
        str eng;
        str rus;
        options opt;

        friend void operator >> (sys::in::pool& pool, entry& x) {
            pool >> x.eng;
            pool >> x.rus;
            pool >> x.opt;
        }

        str html (bool translated) const
        {
            str html;

            if (eng != "")
            {
                str s = eng;
                s.replace_all("||","~");
                s.replace_all("//","~");
                s.replace_all("/", "|");
                str comnt = s.extract_from("%%");
                str sense = s.extract_from("@");

                s.replace_all("(1)", small(small(blue("<sub>1</sub> "))));
                s.replace_all("(2)", small(small(blue("<sub>2</sub> "))));
                s.replace_all("(3)", small(small(blue("<sub>3</sub> "))));
                s.replace_all("(4)", small(small(blue("<sub>4</sub> "))));

                if (s.contains("\\\\"))
                {
                    str uk, us;
                    s.split_by("\\\\", uk, us);
                    uk.strip (); if (not uk.contains("{Br.}")) uk += small(blue(" Br"));
                    us.strip (); if (not us.contains("{Am.}")) us += small(blue(" Am"));
                    s = uk + "<br>" + us;
                }

                s.rebracket("{","}", [](str s){ return small(blue(s)); });

                if (comnt != "")
                {
                    for (str marker: eng_markers)
                    comnt.replace_all(marker, italic(marker));
                    comnt.rebracket("{","}", [](str s){ return green(s); });
                    if (s != "") s += "<br>";
                    s += small(dark(comnt));
                }

                s = bold_italic(s);

                if (s.starts_with(": "))
                s = s.from(2); else
                s = big(s);

                html = s;

                if (opt.external.
                contains("HEAD"))
                html = extracolor(
                    big(html));

                if (opt.external.
                contains("SOUND"))
                html = dark("[" +
                    html + "]");
            }

            if (translated and rus != "")
            {
                str s = rus;
                s.replace_all("||","~");
                s.replace_all("//","~");
                s.replace_all("/", "|");
                str comment = s.extract_from("%%");

                for (str marker: Rus_markers)
                s.replace_all(marker, italic(marker));

                if (comment != "")
                {
                    for (str marker: rus_markers)
                    comment.replace_all(marker, italic(marker));
                    if (s != "") s += "<br>";
                    s += small(gray(comment));
                }
                s = bold_italic(s);

                if (html != "")
                html += "<br>";
                html += dark(s);
            }

            html.rebracket("(",")",[](str s){ return gray("("+s+")"); });
            html.replace_all(u8"→",blue(u8"→"));
            html.replace_all("\\",blue("/"));
            html.replace_all("|" ,blue("/"));
            html.replace_all("[" ,blue("["));
            html.replace_all("]" ,blue("]"));
            html.replace_all("---", mdash);
            html.replace_all("--" , ndash);
            html.replace_all("'", u8"’");
            html.replace_all(" ~", "~");
            html.replace_all("~ ", "~");
            html.replace_all("~","<br>");
            return html;
        }
    };
}