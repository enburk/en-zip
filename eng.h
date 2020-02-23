#pragma once
#include "../ae/library/cpp/aux_abc.h"
#include "../ae/library/cpp/aux_array.h"
#include "../ae/library/cpp/aux_string.h"
#include "../ae/library/cpp/aux_utils.h"
using namespace aux;

namespace eng
{
    struct topic { str header, forms; array<str> content; };
    struct entry { str title; array<topic> topics; };

    inline array<str>   vocabulary;
    inline array<entry> dictionary;

    inline const std::pair<str,str> diacritics
    {
        (char*)u8"ÀàÁáÂâÃãÄäÅå",
        (char*)u8"AaAaAaAaAaAa",
    };
    //u8"Æ Ç È É Ê Ë Ì Í Î Ï Ð Ñ Ò Ó Ô Õ Ö × Ø Ù Ú Û Ü Ý Þ ß"
    //u8"æ ç è é ê ë ì í î ï ð ñ ò ó ô õ ö ÷ ø ù ú û ü ý þ ÿ"
    //u8"Ā ā Ă ă Ą ą Ć ć Ĉ ĉ Ċ ċ Č č Ď ď Đ đ Ē ē Ĕ ĕ Ė ė Ę ę Ě ě Ĝ ĝ Ğ ğ"
    //u8"Ġ ġ Ģ ģ Ĥ ĥ Ħ ħ Ĩ ĩ Ī ī Ĭ ĭ Į į İ ı Ĳ ĳ Ĵ ĵ Ķ ķ ĸ Ĺ ĺ Ļ ļ Ľ ľ Ŀ"
    //u8"ŀ Ł ł Ń ń Ņ ņ Ň ň ŉ Ŋ ŋ Ō ō Ŏ ŏ Ő ő Œ œ Ŕ ŕ Ŗ ŗ Ř ř Ś ś Ŝ ŝ Ş ş"
    //u8"Š š Ţ ţ Ť ť Ŧ ŧ Ũ ũ Ū ū Ŭ ŭ Ů ů Ű ű Ų ų Ŵ ŵ Ŷ ŷ Ÿ Ź ź Ż ż Ž ž ſ"
    //u8"Ǎ ǎ Ǐ ǐ Ǒ ǒ Ǔ ǔ Ǖ ǖ Ǘ ǘ Ǚ ǚ Ǜ ǜ ǝ Ǟ ǟ"
    //u8"Ǡ ǡ Ǣ ǣ Ǥ ǥ Ǧ ǧ Ǩ ǩ Ǫ ǫ Ǭ ǭ Ǵ ǵ Ǻ ǻ"


    // https://www.niso.org/sites/default/files/2017-08/tr03.pdf
    // Guidelines for Alphabetical Arrangement of Letters
    // and Sorting of Numerals and Other Symbols

    int compare (const str & s1, const str & s2)
    {
        int accent = 0;
        auto i1 = s1.begin();
        auto i2 = s2.begin();
        while (true)
        {
            if (i1 == s1.end() && i2 == s2.end()) return accent; 
            if (i1 == s1.end() && i2 != s2.end()) return accent == 0 ? -1 : accent; 
            if (i1 != s1.end() && i2 == s2.end()) return accent == 0 ?  1 : accent; 

            char c1 = *i1++; str cc1;
            char c2 = *i2++; str cc2;

            if ((static_cast<uint8_t>(c1) & 0b10000000) == 0b10000000) { cc1 += c1; c1 = *i1++;
            if ((static_cast<uint8_t>(c1) & 0b11000000) == 0b11000000) { cc1 += c1; c1 = *i1++;
            if ((static_cast<uint8_t>(c1) & 0b11100000) == 0b11100000) { cc1 += c1; c1 = *i1++;
            if ((static_cast<uint8_t>(c1) & 0b11110000) == 0b11110000) { cc1 += c1;
            }}}}
            if ((static_cast<uint8_t>(c2) & 0b10000000) == 0b10000000) { cc2 += c2; c2 = *i2++;
            if ((static_cast<uint8_t>(c2) & 0b11000000) == 0b11000000) { cc2 += c2; c2 = *i2++;
            if ((static_cast<uint8_t>(c2) & 0b11100000) == 0b11100000) { cc2 += c2; c2 = *i2++;
            if ((static_cast<uint8_t>(c2) & 0b11110000) == 0b11110000) { cc2 += c2;
            }}}}

            //

            if (cc1 == "" && cc2 == "") { if (c1 < c2) return -1; if (c1 > c2) return 1; continue; }
            //if (cc1 == "" && cc2 != "")

            if (cc1 == "" && 'A' <= c1 && c1 <= 'Z') { cc1 = c1; c1 = c1 - 'A' + 'a'; }
            if (cc2 == "" && 'A' <= c2 && c2 <= 'Z') { cc2 = c2; c2 = c2 - 'A' + 'a'; }

        }
    }

}
