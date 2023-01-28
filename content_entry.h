#pragma once
#include "eng_abc.h"
#include "content_options.h"
namespace content
{
    struct entry
    {
        str eng,rus;
        str comment;
        options opt;

        entry () = default;
        entry (str s)
        {
            s.strip();
            s.replace_all("\t", " ");
            s.split_by("///", s, comment);
            s.split_by( "#" , s, rus); opt = options(rus);
            s.split_by(" = ", s, rus); eng = s;
            comment.strip();
            eng.strip();
            rus.strip();
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


        struct Eng
        {
            /*
            str en, uk, us, ext, res;
            
            Eng (const str & s = "")
            {
                en = s; en.parse ("%%", en, ext); en.strip (); ext.strip ();

array<str> eng_txt_markers =
array<str>::as ("{Am.}", "{Br.}", "{also}", "{esp.}", "{mainly}", "{usually}", "{informal}", "informal:");

array<str> eng_ext_markers = 
array<str>::as ("Am.", "Br.", "also", "especially", "mainly", "usually", "rarely") +
array<str>::as ("colloquial", "formal", "informal", "idiom", "idiomatic", "literary", "literally", "old-fashioned", "slang", "vulgar") +
array<str>::as ("noun", "verb", "adjective", "adverb", "pronoun", "conjunction", "interjection", "preposition", "prepositional", "determiner", "intensifier") +
array<str>::as ("singular", "plural", "countable", "uncountable", "transitive", "intransitive", "comparative", "superlative") +
array<str>::as ("past tense", "past participle");



                res = en;

                res.replace_all ("{also}"    , ""); res.replace_all ( "<small>" , ""); res.replace_all (u8" → " , ", ");
                res.replace_all ("{esp.}"    , ""); res.replace_all ("</small>" , ""); res.replace_all (u8" ← " , ", ");
                res.replace_all ("{mainly}"  , "");
                res.replace_all ("{usually}" , "");
                res.replace_all ("{informal}", "");
                res.replace_all ( "informal:", "");

                if (en.found ("\\\\"))
                {
                    en.parse ("\\\\", uk, us); uk.strip (); us.strip ();
                    en  = uk.found ("{Br.}") ? uk : uk + " {Br.}"; en += " ~ ";
                    en += us.found ("{Am.}") ? us : us + " {Am.}";

                    uk.replace_all ("{also}"    , ""); us.replace_all ("{also}"    , "");
                    uk.replace_all ("{esp.}"    , ""); us.replace_all ("{esp.}"    , "");
                    uk.replace_all ("{mainly}"  , ""); us.replace_all ("{mainly}"  , "");
                    uk.replace_all ("{usually}" , ""); us.replace_all ("{usually}" , "");
                    uk.replace_all ("{informal}", ""); us.replace_all ("{informal}", "");
                    uk.replace_all ( "informal:", ""); us.replace_all ( "informal:", "");
                }
                else
                {
                    if (res.found ("{Br.}") && res.found ("{Am.}")) throw str ("ENG: %s [BAD UK/US]", *en);
                    if (res.found ("{Br.}")) uk = res; else
                    if (res.found ("{Am.}")) us = res; else uk = us = res;
                }

                res.replace_all ("{Br.}", ""); uk.replace_all ("{Br.}", ""); uk.strip ();
                res.replace_all ("{Am.}", ""); us.replace_all ("{Am.}", ""); us.strip ();

                res = AUX::simple_str (res);
            }

            str show () const
            {
                str s = en; str ss = ext; s.replace_all ( "{EQ}" , "="); ss.replace_all ( "{EQ}" , "=");

                AUX::italize (s,  AUX::eng_txt_markers); // , "<blue>", "</blue>");
                AUX::italize (ss, AUX::eng_ext_markers);

                s.replace_all ("{Br.}"     , "Br.");
                s.replace_all ("{Am.}"     , "Am.");
                s.replace_all ("{also}"    , "also");
                s.replace_all ("{esp.}"    , "especially");
                s.replace_all ("{mainly}"  , "mainly");
                s.replace_all ("{usually}" , "usually");
                s.replace_all ("{informal}", "informal");

                s = AUX::debrace (s);

                s.replace_all ("`", "");

                s = s + "#" + ss;

                s.replace_all ("<noun>"   , "<green>''noun''</green>");
                s.replace_all ("<verb>"   , "<green>''verb''</green>");
                s.replace_all ("<adj.>"   , "<green>''adjective''</green>");
                s.replace_all ("<adv.>"   , "<green>''adverb''</green>");
                s.replace_all ("<int.>"   , "<green>''interjection''</green>");
                s.replace_all ("<conj.>"  , "<green>''conjunction''</green>");
                s.replace_all ("<pronoun>", "<green>''pronoun''</green>");

                s.replace_all ("---", "\xE2" "\x80" "\x94"); // m-dash
                s.replace_all ("--" , "\xE2" "\x80" "\x93"); // n-dash

                s.replace_all ("  ~" , "~"); s.replace_all (" ~" , "~");
                s.replace_all ("~  " , "~"); s.replace_all ("~ " , "~");
        
                int i=0; while (i < s.size () - 1) if (s [i] != '~' || s [i+1] != '~') i++; else { s.erase (i); while (i < s.size () && s [i] == '~') i++; }
        
                s.replace_all ("~", "\n"); s.replace_all ("<br>", "\n");

                s.replace_all ("|", "/");
                s.replace_all ("//","/");

                return s;
            }
            */
        };

        /*
        struct RUS
        {
            str ru, ext; RUS (const str & s = "")
            {
                ru = s; ru.parse ("%%", ru, ext); ru.strip (); ext.strip (); // ::parseback (ru, '(', ext, ')');
            }

            str show () const
            {
                str s = AUX::debrace (ru); str ss = ext;

                AUX::italize (s,  AUX::rus_txt_markers);
                AUX::italize (ss, AUX::rus_ext_markers);

                int b = 0, e = 0; while (true)
                {
                    b = s.findf (b, "("); if (b == -1) break;
                    e = s.findf (b, ")"); if (e == -1) break;

                    s.replace (b, e-b+1, "<gray>" + s.sub (b, e-b+1) + "</gray>"); b += e-b+1 + str ("<gray></gray>").size ();
                }

                s = s + "#" + ss;

                s.replace_all ("---", "\xE2" "\x80" "\x94"); // m-dash
                s.replace_all ("--" , "\xE2" "\x80" "\x93"); // n-dash

                s.replace_all ("  ~" , "~"); s.replace_all (" ~" , "~");
                s.replace_all ("~  " , "~"); s.replace_all ("~ " , "~");

                int i=0; while (i < s.size () - 1) if (s [i] != '~' || s [i+1] != '~') i++; else { s.erase (i); while (i < s.size () && s [i] == '~') i++; }
        
                s.replace_all ("~", "\n"); s.replace_all ("<br>", "\n");

                s.replace_all ("|", "/");
                s.replace_all ("//","/");

                return s;
            }
        };
        */

    };
}
