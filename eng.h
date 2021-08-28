#pragma once
#include <map>
#include "../ae/proto-studio/aux_abc.h"
#include "dat_in.h"
#include "dat_out.h"

namespace eng
{
    inline const std::map<str,str> ligatures
    {
        { (char*)u8"Æ", "AE" }, { (char*)u8"æ", "ae" },
        { (char*)u8"Œ", "OE" }, { (char*)u8"œ", "oe" },
        { (char*)u8"Ĳ", "IJ" }, { (char*)u8"ĳ", "ij" },
    };
    inline const std::map<str,str> diacritics
    {
        { (char*)u8"À", "A" }, { (char*)u8"à", "a" },
        { (char*)u8"Á", "A" }, { (char*)u8"á", "a" },
        { (char*)u8"Â", "A" }, { (char*)u8"â", "a" },
        { (char*)u8"Ã", "A" }, { (char*)u8"ã", "a" },
        { (char*)u8"Ä", "A" }, { (char*)u8"ä", "a" },
        { (char*)u8"Å", "A" }, { (char*)u8"å", "a" },
        { (char*)u8"Ą", "A" }, { (char*)u8"ą", "a" },
        { (char*)u8"Ç", "C" }, { (char*)u8"ç", "c" },
        { (char*)u8"È", "E" }, { (char*)u8"è", "e" },
        { (char*)u8"É", "E" }, { (char*)u8"é", "e" },
        { (char*)u8"Ê", "E" }, { (char*)u8"ê", "e" },
        { (char*)u8"Ë", "E" }, { (char*)u8"ë", "e" },
        { (char*)u8"Ì", "I" }, { (char*)u8"ì", "i" },
        { (char*)u8"Í", "I" }, { (char*)u8"í", "i" },
        { (char*)u8"Î", "I" }, { (char*)u8"î", "i" },
        { (char*)u8"Ï", "I" }, { (char*)u8"ï", "i" },
        { (char*)u8"Ð", "D" }, { (char*)u8"ð", "d" },
        { (char*)u8"Ñ", "N" }, { (char*)u8"ñ", "n" },
        { (char*)u8"Ò", "O" }, { (char*)u8"ò", "o" },
        { (char*)u8"Ó", "O" }, { (char*)u8"ó", "o" },
        { (char*)u8"Ô", "O" }, { (char*)u8"ô", "o" },
        { (char*)u8"Õ", "O" }, { (char*)u8"õ", "o" },
        { (char*)u8"Ö", "O" }, { (char*)u8"ö", "o" },
        { (char*)u8"Ù", "U" }, { (char*)u8"ù", "u" },
        { (char*)u8"Ú", "U" }, { (char*)u8"ú", "u" },
        { (char*)u8"Û", "U" }, { (char*)u8"û", "u" },
        { (char*)u8"Ü", "U" }, { (char*)u8"ü", "u" },
        { (char*)u8"Ý", "Y" }, { (char*)u8"ý", "y" },
    };

    // https://www.niso.org/sites/default/files/2017-08/tr03.pdf
    // Guidelines for Alphabetical Arrangement of Letters
    // and Sorting of Numerals and Other Symbols

    int compare (const str & s1, const str & s2)
    {
        int accent = 0;
        auto i1 = s1.begin();
        auto i2 = s2.begin();
        str ligature_remain[2];

        while (true)
        {
            if (i1 == s1.end() && i2 == s2.end()) return accent; 
            if (i1 == s1.end() && i2 != s2.end()) return -1; 
            if (i1 != s1.end() && i2 == s2.end()) return 1; 

            char cc[2]; str ss[2];

            for (int n=0; n<2; n++)
            {
                char & c = cc[n];
                str  & s = ss[n];
                auto & i = n == 0 ? i1 : i2;

                if (ligature_remain[n] != "") { c =
                    ligature_remain[n][0];
                    ligature_remain[n].erase(0);
                }
                else c = *i++;

                s = c;
                if ((static_cast<uint8_t>(c) & 0b11000000) == 0b11000000) { s += *i++;
                if ((static_cast<uint8_t>(c) & 0b11100000) == 0b11100000) { s += *i++;
                if ((static_cast<uint8_t>(c) & 0b11110000) == 0b11110000) { s += *i++;
                }}}

                if ('a' <= c && c <= 'z') {}
                else
                if ('A' <= c && c <= 'Z') { c = c - 'A' + 'a'; }
                else
                if (auto it = ligatures.find(s); it != ligatures.end())
                {
                    s = it->first;
                    c = it->second[0];
                    ligature_remain[n] = it->second.from(1);
                }
                else
                if (auto it = diacritics.find(s); it != diacritics.end())
                {
                    s = it->first;
                    c = it->second[0];
                }
            }

            auto c1 = static_cast<uint8_t>(cc[0]);
            auto c2 = static_cast<uint8_t>(cc[1]);

            if (c1 < c2) return -1;
            if (c1 > c2) return  1;

            if (accent == 0) accent = 
                ss[0] < ss[1] ? -1 :
                ss[0] > ss[1] ?  1 : 0;
        }
    }

    str asciized (str s)
    {
        /**/ auto j = s.begin();
        for (auto i = s.begin(); i != s.end(); )
        {
            char c = *i++; str g = c;
            uint8_t u = static_cast<uint8_t>(c);
            if ((u & 0b10000000) == 0b00000000) { *j++ = c; continue; }
            if ((u & 0b11000000) == 0b11000000) { if (i == s.end()) break; g += *i++;
            if ((u & 0b11100000) == 0b11100000) { if (i == s.end()) break; g += *i++;
            if ((u & 0b11110000) == 0b11110000) { if (i == s.end()) break; g += *i++;
            }}}
            if (auto k = ligatures .find(g); k != ligatures .end()) g = k->second; else
            if (auto k = diacritics.find(g); k != diacritics.end()) g = k->second; else
            {}
            for (char c : g) *j++ = c;
        }
        s.erase(j, s.end());
        return s;
    }

    auto less = [](const str & a, const str & b) { return compare(a, b) < 0; };

    auto less_case_insensitive = [](const str & a, const str & b) {
        return compare(
            asciized(a).ascii_lowercased(),
            asciized(b).ascii_lowercased())
            < 0; };

    auto equal_case_insensitive = [](const str & a, const str & b) {
        return compare(
            asciized(a).ascii_lowercased(),
            asciized(b).ascii_lowercased())
            == 0; };
}
