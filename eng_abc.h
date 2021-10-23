#pragma once
#include <map>
#include "abc.h"
namespace eng
{
    const std::vector<std::pair<str,str>> ligatures
    {
        { (char*)u8"Æ", "AE" }, { (char*)u8"æ", "ae" },
        { (char*)u8"Œ", "OE" }, { (char*)u8"œ", "oe" },
        { (char*)u8"Ĳ", "IJ" }, { (char*)u8"ĳ", "ij" },
    };
    const std::vector<std::pair<str,str>> diacritics
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

    auto ligature (str const& s) {
        auto it = std::ranges::find(
        ligatures, s, &std::pair<str,str>::first);
        return it != ligatures.end() and it->first == s ?
        std::optional<decltype(it)>(it) : std::nullopt; };

    auto diacritic (str const& s) {
        auto it = std::ranges::find(
        diacritics, s, &std::pair<str,str>::first);
        return it != diacritics.end() and it->first == s ?
        std::optional<decltype(it)>(it) : std::nullopt; };

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
            bool end1 = i1 == s1.end() and ligature_remain[0] == "";
            bool end2 = i2 == s2.end() and ligature_remain[1] == "";

            if (end1 and end2) return accent; 
            if (end1 and not end2) return -1; 
            if (end2 and not end1) return  1; 

            char cc[2]; str ss[2]; int weight[2]{}; 

            for (int n: {0, 1})
            {
                char & c = cc[n];
                str  & s = ss[n];
                int  & w = weight[n];
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

                // A < Ä < a < ä
                // AE < Æ < ÄË < ae < æ < äë

                if ('a' <= c && c <= 'z') {
                    w = 100'000;
                }
                else
                if ('A' <= c && c <= 'Z') {
                    c = c - 'A' + 'a';
                    w = 0;
                }
                else
                if (auto it = ligature(s); it)
                {
                    int i = (int)(*it - ligatures.begin());
                    w = 101'000 + i;
                    c = (*it)->second[0];
                    ligature_remain[n] = (*it)->second.from(1);
                    if ('A' <= c && c <= 'Z') {
                        c = c - 'A' + 'a';
                        w = 1'000 + i;
                    }
                }
                else
                if (auto it = diacritic(s); it)
                {
                    int i = (int)(*it - diacritics.begin());
                    w = 102'000 + i;
                    c = (*it)->second[0];
                    if ('A' <= c && c <= 'Z') {
                        c = c - 'A' + 'a';
                        w = 2'000 + i;
                    }
                }
            }

            auto c1 = static_cast<uint8_t>(cc[0]);
            auto c2 = static_cast<uint8_t>(cc[1]);

            if (c1 < c2) return -1;
            if (c1 > c2) return  1;

            if (accent == 0) accent = 
            weight[0] < weight[1] ? -1 :
            weight[0] > weight[1] ?  1 : 0;
        }
    }

    str asciized (str s)
    {
        /**/ auto j = s.begin();
        for (auto i = s.begin(); i != s.end(); )
        {
            char c = *i++; str g = c;
            uint8_t u = static_cast<uint8_t>(c);
            if ((u & 0b11000000) == 0b11000000) { g += *i++;
            if ((u & 0b11100000) == 0b11100000) { g += *i++;
            if ((u & 0b11110000) == 0b11110000) { g += *i++;
            }}}
            if (auto k = ligature (g); k) g = (*k)->second; else
            if (auto k = diacritic(g); k) g = (*k)->second; else
            {}
            for (char c : g) *j++ = c;
        }
        s.erase(j, s.end());
        return s;
    }

    str lowercased (str s)
    {
        /**/ auto j = s.begin();
        for (auto i = s.begin(); i != s.end(); )
        {
            char c = *i++; str g = c;
            uint8_t u = static_cast<uint8_t>(c);
            if ((u & 0b11000000) == 0b11000000) { if (i == s.end()) break; g += *i++;
            if ((u & 0b11100000) == 0b11100000) { if (i == s.end()) break; g += *i++;
            if ((u & 0b11110000) == 0b11110000) { if (i == s.end()) break; g += *i++;
            }}}
            if ('a' <= c && c <= 'z') {} else
            if ('A' <= c && c <= 'Z') {
                c = c - 'A' + 'a';
                g[0] = c;
            }
            else
            if (auto k = ligature(g); k)
            {
                auto n = (*k) - ligatures.begin();
                if ((n % 2) == 0) g = ligatures[n+1].first;
            }
            else
            if (auto k = diacritic(g); k)
            {
                auto n = (*k) - diacritics.begin();
                if ((n % 2) == 0) g = diacritics[n+1].first;
            }
            for (char c : g) *j++ = c;
        }
        s.erase(j, s.end());
        return s;
    }

    str uppercased (str s)
    {
        /**/ auto j = s.begin();
        for (auto i = s.begin(); i != s.end(); )
        {
            char c = *i++; str g = c;
            uint8_t u = static_cast<uint8_t>(c);
            if ((u & 0b11000000) == 0b11000000) { if (i == s.end()) break; g += *i++;
            if ((u & 0b11100000) == 0b11100000) { if (i == s.end()) break; g += *i++;
            if ((u & 0b11110000) == 0b11110000) { if (i == s.end()) break; g += *i++;
            }}}
            if ('A' <= c && c <= 'Z') {} else
            if ('a' <= c && c <= 'z') {
                c = c - 'a' + 'A';
                g[0] = c;
            }
            else
            if (auto k = ligature(g); k)
            {
                auto n = (*k) - ligatures.begin();
                if ((n % 2) == 1) g = ligatures[n-1].first;
            }
            else
            if (auto k = diacritic(g); k)
            {
                auto n = (*k) - diacritics.begin();
                if ((n % 2) == 1) g = diacritics[n-1].first;
            }
            for (char c : g) *j++ = c;
        }
        s.erase(j, s.end());
        return s;
    }

    auto less = []
    (str const& a, str const& b) {
        return compare(a, b) < 0; };

    auto less_case_insensitive = []
    (str const& a, str const& b) {
        return compare(
            lowercased(a),
            lowercased(b))
            < 0; };

    auto equal_case_insensitive = []
    (str const& a, str const& b) {
        return compare(
            lowercased(a),
            lowercased(b))
            == 0; };
}
