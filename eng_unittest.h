#pragma once
#include "eng_parser.h"
namespace eng::unittest
{
    using aux::unittest::out;
    using aux::unittest::test;
    using aux::unittest::string;
    using aux::unittest::vector;
    using aux::unittest::expect_;

    bool proceed (vocabulary& vocabulary, gui::console& report) try
    {
        test("UNITTEST.sort"); 
        {
            oops(out(eng::less("A", "a"))) {"1"};
            oops(out(eng::less("a", "a"))) {"0"};
            oops(out(eng::less("A", "A"))) {"0"};
            oops(out(eng::less("a", "A"))) {"0"};

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
            
            #pragma warning(pop)
        }
        test("");

        if (aux::unittest::all_ok) {
        report << bold(green("UNITTEST OK"));
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
