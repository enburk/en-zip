#pragma once
#include <execution>
#include "data_out.h"
#include "app_dic.h"
namespace studio::build
{
    bool dictionary_update (gui::console & out, gui::console & err)
    {
        using std::filesystem::path;
        path src = "../en-wiktionary/enwiktionary-data.txt";
        path dir = "../data";

        if (true)
        if (std::filesystem::exists         (dir / "dictionary.dat") &&
            std::filesystem::exists         (dir / "vocabulary.dat") &&
            std::filesystem::last_write_time(dir / "vocabulary.dat") >
            std::filesystem::last_write_time(src))
            return false;

        out << "dictionary update...";

        std::filesystem::create_directories(dir);

        eng::dictionary dictionary;
        dictionary.data.reserve(1024*1024);

        str line, s1, s2;
        std::ifstream ifstream (src);
        while (std::getline(ifstream, line))
        {
            auto& data = dictionary.data;

            if (line == "") continue;
            if (line.starts_with("  == ")) {
                line.split_by   ("  == ", s1, line);
                line.split_by   ( " == ", s1, s2);
                data.back().topics += eng::dictionary::topic{};
                data.back().topics.back().header = s1;
                data.back().topics.back().forms = s2;
            }
            else
            if (line.starts_with("     # ")) {
                line.split_by   ("     # ", s1, s2);
                data.back().topics.back().content += s2;
            }
            else
            {
                data += eng::dictionary::entry{line};
                if (data.size() % 100'000 == 0)
                    out << "read " + std::to_string(
                    data.size()) + " entries";
            }
        }

        out << "dictionary sort...";
        std::sort(std::execution::par_unseq,
            dictionary.data.begin(), dictionary.data.end(),
            [](auto const& a, auto const& b) { return
            eng::less(a.title, b.title); });
        out << "dictionary sort ok";

        eng::vocabulary vocabulary(dictionary);

        array<str> unicodes;
        array<str> sortings;
        bool next_line = false;

        for (auto [i, entry] : enumerate(vocabulary.data))
        {
            bool unicode = false;
            bool sorting = false;
            for (char c : entry.title)
            {
                auto b = static_cast<uint8_t>(c);
                if (b & 0b10000000) unicode = true; else
                if (c >= 'A') if (c < 'a' or 'z' < c)
                    sorting = true;
            }
            if (unicode) sorting = true;
            if (unicode) unicodes += entry.title;

            if (sorting and not next_line and i > 0)
                sortings += vocabulary[i-1].title;
            if (sorting || next_line)
                sortings += entry.title;

            next_line = sorting;
        }

        std::ofstream ofstream;
        
        ofstream = std::ofstream(dir/"vocabulary.txt");
        for (auto& e: vocabulary.data)
            ofstream << e.title << "\n";

        ofstream = std::ofstream(dir/"vocabulary_unicode.txt");
        for (auto& s: unicodes)
            ofstream << s << "\n";

        ofstream = std::ofstream(dir/"vocabulary_sorting.txt");
        for (auto& s : sortings)
            ofstream << s << "\n";

        ofstream = std::ofstream(dir/"vocabulary_problem.txt");

        out << "dictionary analysis...";

        for (int i=0; i<dictionary.data.size(); i++)
        {
            auto & topics =
            dictionary[i].topics;
            if (topics.size() != 1 or
                topics.front().header != ">>>")
                continue; // it's not a redirect

            str redirect = topics[0].forms;
            if (redirect == vocabulary[i].title) {
                ofstream << "REDIRECT: TO ITSELF: " <<
                dictionary[i].title << " >>> " <<
                redirect << "\n";
                continue; }

            auto index = vocabulary.index(redirect);
            if (not index) {
                ofstream << "REDIRECT: NOT FOUND: " <<
                dictionary[i].title << " >>> " <<
                redirect << "\n";
                continue; }

            int n = *index;
            if (dictionary[n].title != redirect)
                ofstream << "REDIRECT: MISMATCH: " <<
                dictionary[i].title << " >>> " <<
                dictionary[n].title << " <<< " <<
                redirect << "\n";

            auto & topics2 =
            dictionary[n].topics;
            if (topics2.size() == 1 and
                topics2.front().header == ">>>") {
                ofstream << "REDIRECT: TRANSIT: " <<
                dictionary[i].title << " >>> " <<
                redirect << "\n";
                continue; }

            dictionary[n].redirects += i;
            vocabulary[i].redirect = n;
        }

        dat::out::file dictionary_out(dir/"dictionary.dat");

        for (int i=0; i<dictionary.data.size(); i++)
        {
            if ((i+1) % 100'000 == 0) out <<
                "written " + std::to_string
                    (i+1) + " entries";

            if (
            vocabulary[i].redirect >= 0) continue;
            vocabulary[i].offset = dictionary_out.size;
            dictionary[i] >>       dictionary_out;
            vocabulary[i].length = dictionary_out.size -
            vocabulary[i].offset;
        }

        ofstream = std::ofstream(dir/"vocabulary_redirect.txt");

        for (int i=0; i<dictionary.data.size(); i++)
        {
            if (vocabulary[i].redirect < 0) continue;

            int n = vocabulary[i].redirect;

            vocabulary[i].offset = vocabulary[n].offset;
            vocabulary[i].length = vocabulary[n].length;

            ofstream << 
            dictionary[i].title << " >>> " <<
            dictionary[n].title << "\n";
        }

        dat::out::file vocabulary_out(dir/"vocabulary.dat");
        vocabulary_out << 0x12345678; // endianness
        vocabulary_out << vocabulary.data.size();
        for (auto& entry: vocabulary.data)
            entry >> vocabulary_out;

        dictionary_out.close();
        vocabulary_out.close();

        out << "vocabulary check...";
        ofstream = std::ofstream(dir/"vocabulary_check.txt");
        eng::vocabulary Vocabulary(dir/"vocabulary.dat");

        if (Vocabulary.data.size() !=
            vocabulary.data.size()) ofstream << "SIZE: " <<
            Vocabulary.data.size() << " != " <<
            vocabulary.data.size() << "\n";

        for (int i=0; i<vocabulary.data.size(); i++)
        {
            const auto & a = Vocabulary[i];
            const auto & b = vocabulary[i];
            if (a.title    != b.title
            or  a.offset   != b.offset   
            or  a.length   != b.length   
            or  a.redirect != b.redirect 
                ) ofstream <<
                a.title    << " [" <<
                a.offset   << ", " <<
                a.length   << ", " <<
                a.redirect << "] != " <<
                b.title    << " [" << 
                b.offset   << ", " << 
                b.length   << ", " << 
                b.redirect << "]\n";
        }

        out << "vocabulary print...";
        ofstream = std::ofstream(dir/"vocabulary_numbers.txt");

        for (int i=0; i<vocabulary.data.size(); i++)
        {
            const auto & a = vocabulary[i];
            ofstream  << i << " "  <<
                a.title    << " [" <<
                a.offset   << ", " <<
                a.length   << ", " <<
                a.redirect << "]\n";
        }

        out << "dictionary update ok";
        out << "";

        return true;
    }
}
