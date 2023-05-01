#pragma once
#include "eng_abc.h"
#include "content_options.h"
namespace content
{
    array<str> Eng_markers = str(
    "{Am.}, {Br.}, {also}, {esp.}, {mainly}, {usually}, {informal}, {informal:}"
    ).split_by(", ");

    array<str> eng_markers = str("Am., Br., "
    "also, especially, mainly, usually, rarely, "
    "colloquial, formal, informal, idiom, idiomatic, "
    "literary, literally, old-fashioned, slang, vulgar, "
    "singular, plural, countable, uncountable, comparative, superlative, "
    "transitive, intransitive, past tense, past participle"
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
        array<str> vocabulary;
        array<str> errors;
        int line = 0;
        str link;

        entry () = default;
        entry (str s, path path, int line) :  line{line}
        {
            link = str(path) + "|" +
                std::to_string(line);

            s.strip();
            s.replace_all("\t", " ");

            eng = s;

            if (s == ""
            or  s.starts_with("---")
            or  s.starts_with("==="))
                return;

            comment = s.extract_from("///");

            opt = s.extract_from("#");
            rus = s.extract_from("=");
            eng = s;

            for (str o: opt.unknown)
            errors += "UNKNOWN OPTION: " + o;

            if (s.starts_with(": ")) {
                en += s.from(2);
                return; }

            str
            commt = s.extract_from("%%");
            sense = s.extract_from("@");

            if (s.contains(
            one_of ("/|{}()[]")))
            anomaly = "{}()[]";

            if (s.contains("\\\\"))
            {
                anomaly = "Br/Am";
                for (str marker: Eng_markers)
                s.replace_all(marker, "");
                s.replace_all("~" , "");

                str s1, s2;
                s.split_by("\\\\", s1, s2);
                s1.strip (); uk += s1;
                s2.strip (); us += s2;
            }
            else
            {
                bool br = s.contains("{Br.}");
                bool am = s.contains("{Am.}");

                for (str marker: Eng_markers)
                s.replace_all(marker, "");
                s.replace_all("~~" , "/");
                s.replace_all("~" , "");

                s.debracket("(",")"); str ss = s;
                s.debracket("[","]");
                s.canonicalize();

                if (br or  am) anomaly = "Br/Am";
                if (br and am) errors += "Br. & Am."; else
                if (br) uk += s.split_by("/"); else
                if (am) us += s.split_by("/"); else
                        en += s.split_by("/");

                // beluga [whale]
                ss.replace_all("[" , "");
                ss.replace_all("]" , "");
                ss.canonicalize();
                if (ss != s) {
                if (br) uk += ss; else
                if (am) us += ss; else
                        en += ss; }
            }

            if (s.contains(
            one_of   ("{}()[]")))
            errors += "{}()[]";

            for (str ss: en*uk*us)
            for (str s: ss.split_by("|"))
            vocabulary += s;
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

            str s = eng;
            str comment, sense;
            s.replace_all("/","|");
            s.split_by("%%", s, comment);
            s.split_by("@" , s, sense);
            s.strip(); comment.strip();

            s.replace_all("(1)", small(blue("<sub>1</sub>")));
            s.replace_all("(2)", small(blue("<sub>2</sub>")));
            s.replace_all("(3)", small(blue("<sub>3</sub>")));
            s.replace_all("(4)", small(blue("<sub>4</sub>")));

            if (s.contains("\\\\"))
            {
                str uk, us;
                s.split_by("\\\\", uk, us);
                uk.strip (); uk += small(blue(" Br"));
                us.strip (); us += small(blue(" Am"));
                s = uk + "<br>" + us;
            }

            s.rebracket("{","}",[](str s){ return small(blue(s)); });

            if (comment != "")
            {
                for (str marker: eng_markers)
                comment.replace_all(marker, italic(marker));
                s += "<br>" + small(dark(comment));
            }

            s = bold_italic(s);

            html = big(s);

            if (opt.external.
            contains("HEAD"))
            html = big(html);

            if (translated and rus != "")
            {
                s = rus;
                s.replace_all("/","|");
                s.split_by("%%", s, comment);
                s.strip(); comment.strip();

                for (str marker: Rus_markers)
                s.replace_all(marker, italic(marker));

                if (comment != "")
                {
                    for (str marker: rus_markers)
                    comment.replace_all(marker, italic(marker));
                    s += "<br>" + small(gray(comment));
                }
                s = bold_italic(s);

                html += "<br>" + dark(s);
            }

            html.rebracket("(",")",[](str s){ return gray("("+s+")"); });
            html.replace_all("|",blue("/"));
            html.replace_all("[",blue("["));
            html.replace_all("]",blue("]"));
            html.replace_all("---", mdash);
            html.replace_all("--" , ndash);
            html.replace_all("~~", "~");
            html.replace_all(" ~", "~");
            html.replace_all("~ ", "~");
            html.replace_all("~","<br>");
            return html;
        }
    };
}