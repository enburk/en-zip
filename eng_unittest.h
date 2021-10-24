#pragma once
#include "eng_parser.h"
namespace eng::unittest
{
    using aux::unittest::out;
    using aux::unittest::test;
    using aux::unittest::string;
    using aux::unittest::vector;
    using aux::unittest::expect_;

    void heavy (gui::console& report) try
    {
        std::filesystem::path path = "../data/vocabulary.dat";
        str font  = "<font color=#808080 face=\"monospace\">";
        auto N = [](auto n){ return std::to_string(n); };
        auto T = [](auto t){ return sys::format(t); };

        test("UNITTEST.HEAVY.load1"); 
        {
            sys::timing t0; vocabulary_basic  v1(path);
            sys::timing t1; vocabulary_hashed v2(path);
            sys::timing t2; vocabulary_cached v3(path);
            sys::timing t3; vocabulary_trie   v4(path);
            sys::timing t4;
            report << font +
            "load1 basic  " + sys::format(t1-t0) + " sec<br>"+
            "load1 hashed " + sys::format(t2-t1) + " sec<br>"+
            "load1 cashed " + sys::format(t3-t2) + " sec<br>"+
            "load1 trie   " + sys::format(t4-t3) + " sec<br>"+
            "</font>";
            oops(out("ok")) {"ok"};
        }
        vocabulary_basic  v1(path);
        vocabulary_hashed v2(path);
        vocabulary_cached v3(path);
        vocabulary_trie   v4(path);

        test("UNITTEST.HEAVY.index"); 
        {
            int n = 100'000;
            array<str> ss; ss.resize(n);
            for (int i=0; i<n; i++)
            for (int j=0; j<aux::random(0, 15); j++)
            ss[i] += (char)(aux::random<int>('a', 'z'));
            unsigned n1 = 0, u1 = 0;
            unsigned n2 = 0, u2 = 0;
            unsigned n3 = 0, u3 = 0;
            unsigned n4 = 0, u4 = 0;
            sys::timing t0; for (str s: ss) if (auto r = v1.index(s); r) { n1++; u1 += *r; }
            sys::timing t1; for (str s: ss) if (auto r = v2.index(s); r) { n2++; u2 += *r; }
            sys::timing t2; for (str s: ss) if (auto r = v3.index(s); r) { n3++; u3 += *r; }
            sys::timing t3; for (str s: ss) if (auto r = v4.index(s); r) { n4++; u4 += *r; }
            sys::timing t4;
            report << font +
            "index basic  " + T(t1-t0) + " sec " + N(n1) + ": "+ N(u1) + "<br>"+
            "index hashed " + T(t2-t1) + " sec " + N(n2) + ": "+ N(u2) + "<br>"+
            "index cashed " + T(t3-t2) + " sec " + N(n3) + ": "+ N(u3) + "<br>"+
            "index trie   " + T(t4-t3) + " sec " + N(n4) + ": "+ N(u4) + "<br>"+
            "</font>";
            oops(out(n1 == n2 and n2 == n3 and n3 == n4)) {"1"};
            oops(out(u1 == u2 and u2 == u3 and u3 == u4)) {"1"};
        }
        test("UNITTEST.HEAVY.lower"); 
        {
            int n = 100'000;
            array<str> ss; ss.resize(n);
            for (int i=0; i<n; i++)
            for (int j=0; j<aux::random(0, 15); j++)
            ss[i] += (char)(aux::random<int>('a', 'z'));
            unsigned u1 = 0;
            unsigned u2 = 0;
            unsigned u3 = 0;
            unsigned u4 = 0;
            sys::timing t0; for (str s: ss) u1 += v1.lower_bound(s);
            sys::timing t1; for (str s: ss) u2 += v2.lower_bound(s);
            sys::timing t2; for (str s: ss) u3 += v3.lower_bound(s);
            sys::timing t3; for (str s: ss) u4 += v4.lower_bound(s);
            sys::timing t4;
            report << font +
            "lower basic  " + T(t1-t0) + " sec " + N(u1) + "<br>"+
            "lower hashed " + T(t2-t1) + " sec " + N(u2) + "<br>"+
            "lower cashed " + T(t3-t2) + " sec " + N(u3) + "<br>"+
            "lower trie   " + T(t4-t3) + " sec " + N(u4) + "<br>"+
            "</font>";
            oops(out(u1 == u2 and u2 == u3 and u3 == u4)) {"1"};
        }
        test("");
        report << aux::unittest::results;
        report << bold(green("HEAVY UNITTEST OK"));
    }
    catch(aux::unittest::assertion_failed)
    {
        report << aux::unittest::results;
        report << bold(red("HEAVY UNITTEST CRASH"));
    }

    bool smoke (vocabulary& vocabulary, gui::console& report) try
    {
        test("UNITTEST.sort"); 
        {
            oops(out(eng::less("A", "a"))) {"1"};
            oops(out(eng::less("a", "a"))) {"0"};
            oops(out(eng::less("A", "A"))) {"0"};
            oops(out(eng::less("a", "A"))) {"0"};

            oops(out(eng::less("911", "911" ))) {"0"};
            oops(out(eng::less("911", "911s"))) {"1"};
            oops(out(eng::less("911", "912" ))) {"1"};

            oops(out(eng::less("?",  "?!" ))) {"1"};
            oops(out(eng::less("?!", "??" ))) {"1"};
            oops(out(eng::less("??", "???"))) {"1"};

            str _A_ = (char*)(u8"Ä");
            str _a_ = (char*)(u8"ä");

            oops(out(eng::less("A", _A_))) {"1"};
            oops(out(eng::less(_A_, "a"))) {"1"};
            oops(out(eng::less("a", _a_))) {"1"};
            oops(out(eng::less(_a_, "B"))) {"1"};

            #pragma warning(push)
            #pragma warning(disable: 4566)
            oops(out(eng::less((char*)(u8"a" ), (char*)(u8"a ")))) {"1"};
            oops(out(eng::less((char*)(u8"a "), (char*)(u8"AE")))) {"1"};
            oops(out(eng::less((char*)(u8"AE"), (char*)(u8"AË")))) {"1"};
            oops(out(eng::less((char*)(u8"AË"), (char*)(u8"Ae")))) {"1"};
            oops(out(eng::less((char*)(u8"Ae"), (char*)(u8"Aë")))) {"1"};
            oops(out(eng::less((char*)(u8"Aë"), (char*)(u8"Æ" )))) {"1"};
            oops(out(eng::less((char*)(u8"Æ" ), (char*)(u8"ÄE")))) {"1"};
            oops(out(eng::less((char*)(u8"ÄE"), (char*)(u8"ÄË")))) {"1"};
            oops(out(eng::less((char*)(u8"ÄË"), (char*)(u8"Äe")))) {"1"};
            oops(out(eng::less((char*)(u8"Äe"), (char*)(u8"Äë")))) {"1"};
            oops(out(eng::less((char*)(u8"Äë"), (char*)(u8"aE")))) {"1"};
            oops(out(eng::less((char*)(u8"aE"), (char*)(u8"aË")))) {"1"};
            oops(out(eng::less((char*)(u8"aË"), (char*)(u8"ae")))) {"1"};
            oops(out(eng::less((char*)(u8"ae"), (char*)(u8"aë")))) {"1"};
            oops(out(eng::less((char*)(u8"aë"), (char*)(u8"æ" )))) {"1"};
            oops(out(eng::less((char*)(u8"æ" ), (char*)(u8"äE")))) {"1"};
            oops(out(eng::less((char*)(u8"äE"), (char*)(u8"äe")))) {"1"};
            oops(out(eng::less((char*)(u8"äe"), (char*)(u8"äë")))) {"1"};
            oops(out(eng::less((char*)(u8"äë"), (char*)(u8"af")))) {"1"};
            oops(out(eng::less((char*)(u8"af"), (char*)(u8"B" )))) {"1"};

            oops(out(eng::less((char*)(u8"a "), (char*)(u8"a" )))) {"0"};
            oops(out(eng::less((char*)(u8"AE"), (char*)(u8"a ")))) {"0"};
            oops(out(eng::less((char*)(u8"AË"), (char*)(u8"AE")))) {"0"};
            oops(out(eng::less((char*)(u8"Ae"), (char*)(u8"AË")))) {"0"};
            oops(out(eng::less((char*)(u8"Aë"), (char*)(u8"Ae")))) {"0"};
            oops(out(eng::less((char*)(u8"Æ" ), (char*)(u8"Aë")))) {"0"};
            oops(out(eng::less((char*)(u8"ÄE"), (char*)(u8"Æ" )))) {"0"};
            oops(out(eng::less((char*)(u8"ÄË"), (char*)(u8"ÄE")))) {"0"};
            oops(out(eng::less((char*)(u8"Äe"), (char*)(u8"ÄË")))) {"0"};
            oops(out(eng::less((char*)(u8"Äë"), (char*)(u8"Äe")))) {"0"};
            oops(out(eng::less((char*)(u8"aE"), (char*)(u8"Äë")))) {"0"};
            oops(out(eng::less((char*)(u8"aË"), (char*)(u8"aE")))) {"0"};
            oops(out(eng::less((char*)(u8"ae"), (char*)(u8"aË")))) {"0"};
            oops(out(eng::less((char*)(u8"aë"), (char*)(u8"ae")))) {"0"};
            oops(out(eng::less((char*)(u8"æ" ), (char*)(u8"aë")))) {"0"};
            oops(out(eng::less((char*)(u8"äE"), (char*)(u8"æ" )))) {"0"};
            oops(out(eng::less((char*)(u8"äe"), (char*)(u8"äE")))) {"0"};
            oops(out(eng::less((char*)(u8"äë"), (char*)(u8"äe")))) {"0"};
            oops(out(eng::less((char*)(u8"af"), (char*)(u8"äë")))) {"0"};
            oops(out(eng::less((char*)(u8"B" ), (char*)(u8"af")))) {"0"};

            oops(out(eng::less((char*)(u8"å" ), (char*)(u8"å")))) {"0"};
            oops(out(eng::less((char*)(u8"å" ), (char*)(u8"à")))) {"0"};
            oops(out(eng::less((char*)(u8"à" ), (char*)(u8"å")))) {"1"};
            oops(out(eng::less((char*)(u8"à" ), (char*)(u8"à")))) {"0"};
            #pragma warning(pop)
        }
        test("UNITTEST.asciized"); 
        {
            #pragma warning(push)
            #pragma warning(disable: 4566)
            oops(out(eng::asciized((char*)(u8"AEÆÄËaeæäë")))) {"AEAEAEaeaeae"};
            oops(out(eng::asciized((char*)(u8"AÆÄEËaeæäë")))) {"AAEAEEaeaeae"};
            oops(out(eng::asciized((char*)(u8"ÆAÄËEaeæäë")))) {"AEAAEEaeaeae"};
            oops(out(eng::asciized((char*)(u8"ÆÄAËaEeæäë")))) {"AEAAEaEeaeae"};
            oops(out(eng::asciized((char*)(u8"ÄÆËAaeEæäë")))) {"AAEEAaeEaeae"};
            oops(out(eng::asciized((char*)(u8"ÄËÆaAeæEäë")))) {"AEAEaAeaeEae"};
            oops(out(eng::asciized((char*)(u8"ÄËaÆeAæäEë")))) {"AEaAEeAaeaEe"};
            #pragma warning(pop)

        }
        test("UNITTEST.lowercased"); 
        {
            #pragma warning(push)
            #pragma warning(disable: 4566)
            oops(out(eng::lowercased((char*)(u8"AEÆÄËaeæäë")))) {(char*)(u8"aeæäëaeæäë")};
            oops(out(eng::lowercased((char*)(u8"AÆÄEËaeæäë")))) {(char*)(u8"aæäeëaeæäë")};
            oops(out(eng::lowercased((char*)(u8"ÆAÄËEaeæäë")))) {(char*)(u8"æaäëeaeæäë")};
            oops(out(eng::lowercased((char*)(u8"ÆÄAËaEeæäë")))) {(char*)(u8"æäaëaeeæäë")};
            oops(out(eng::lowercased((char*)(u8"ÄÆËAaeEæäë")))) {(char*)(u8"äæëaaeeæäë")};
            oops(out(eng::lowercased((char*)(u8"ÄËÆaAeæEäë")))) {(char*)(u8"äëæaaeæeäë")};
            oops(out(eng::lowercased((char*)(u8"ÄËaÆeAæäEë")))) {(char*)(u8"äëaæeaæäeë")};
            #pragma warning(pop)
        }
        test("UNITTEST.lower.bound"); 
        {
            auto lower = [&vocabulary](str s){ return
                vocabulary[vocabulary.
                lower_bound_case_insensitive(s)].title; };

            oops(out(lower("PSYCHE"))) {"Psyche"};
            oops(out(lower("PSyche"))) {"Psyche"};
            oops(out(lower("Psyche"))) {"Psyche"};
            oops(out(lower("psyche"))) {"psyche"};
            oops(out(lower("psychE"))) {"psyche"};
            oops(out(lower("Psyche Knot"))) {"Psyche knot"};
            oops(out(lower("Psyche knot"))) {"Psyche knot"};
            oops(out(lower("psyche knot"))) {"Psyche knot"};
            oops(out(lower("PSyched"))) {"psyched"};
            oops(out(lower("Psyched"))) {"psyched"};
            oops(out(lower("psyched"))) {"psyched"};
            oops(out(lower("AN"))) {"AN"};
            oops(out(lower("An"))) {"An"};
            oops(out(lower("aN"))) {"aN"};
            oops(out(lower("an"))) {"an"};
            oops(out(lower("!"))) {"!"};
            oops(out(lower(" "))) {"!"};
            oops(out(lower("zzzzzzzz"))) {"zzzzzzzz"};
            #pragma warning(push)
            #pragma warning(disable: 4566)
            oops(out(lower((char*)(u8"Ⅴ⁷/Ⅴ")))) {(char*)(u8"Ⅴ⁷/Ⅴ")};
            oops(out(lower((char*)(u8"Ⅴ⁷Ⅴ⁷")))) {(char*)(u8"Ⅴ⁷/Ⅴ")};
            #pragma warning(pop)
        }

        auto matches = [&](str s){
            array<doc::text::token> tokens;
            auto text = doc::text::text(s);
            for (auto t: doc::html::lexica::parse(text)) tokens += t;
            auto matches = parser::matches(vocabulary, tokens);
            array<str> ss; for (auto m: matches) ss += m.text;
            out(ss);
        };

        test("UNITTEST.matches"); 
        {
            oops(matches("AN")) {"AN, an"}; 
            oops(matches("An")) {"An, an"}; 
            oops(matches("aN")) {"aN, an"}; 
            oops(matches("an")) {"an"}; 
            oops(matches("an al")) {"an, al"}; 
            oops(matches("an all")) {"an, an all, all"}; 
            oops(matches("an alll")) {"an"}; 
            oops(matches("an apple a day")) {"an, an apple a day, apple, apple a day, a, day"}; 
            oops(matches("AN Other")) {"AN, an, AN Other, other"}; 
            oops(matches("Anabaena")) {"Anabaena"}; 
            oops(matches("anabaena")) {""}; 
        }


        auto entries = [&](str s){ 
            auto ee = parser::entries(vocabulary, s);
            std::ranges::sort(ee, eng::less);
            out(ee);
        };

        test("UNITTEST.entries"); 
        {
            #pragma warning(push)
            #pragma warning(disable: 4566)
            oops(entries("In New York")) {"in, new, New York, york"}; 
            oops(entries("In _New _York")) {"in, New, New York, York"}; 
            oops(entries((char*)(u8"æsthete"))) {(char*)(u8"æsthete")}; 
            oops(entries((char*)(u8"æsteem"))) {(char*)(u8"æsteem")}; 
            oops(entries((char*)(u8"Æsop"))) {(char*)(u8"Æsop")}; 
            oops(entries("Cat and cat")) {"and, cat"}; 
            oops(entries("Cat and Cat")) {"and, cat"}; 
            oops(entries("Cat and _Cat")) {"and, Cat, cat"}; 
            #pragma warning(pop)
        }
        test("");

        if (aux::unittest::all_ok) {
        report << bold(green("UNITTEST OK"));
        if (true) { heavy(report); return false; }
        return true; }

        report << aux::unittest::results;
        return false;
    }
    catch(aux::unittest::assertion_failed)
    {
        report << aux::unittest::results;
        report << bold(red("UNITTEST CRASH"));
        return false;
    }
}
