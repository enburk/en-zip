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

        array<eng::dictionary::entry> dictionary;
        dictionary.reserve(1024*1024);

        str line, s1, s2;
        std::ifstream ifstream (src);
        while (std::getline(ifstream, line))
        {
            if (line == "") continue;
            if (line.starts_with("  == ")) {
                line.split_by   ("  == ", s1, line);
                line.split_by   ( " == ", s1, s2);
                dictionary.back().topics += eng::dictionary::topic{};
                dictionary.back().topics.back().header = s1;
                dictionary.back().topics.back().forms = s2;
            }
            else
            if (line.starts_with("     # ")) {
                line.split_by   ("     # ", s1, s2);
                dictionary.back().topics.back().content += s2;
            }
            else
            {
                dictionary += eng::dictionary::entry{line};
                if (dictionary.size() % 100'000 == 0)
                    out << "read " + std::to_string(
                    dictionary.size()) + " entries";
            }
        }

        out << "dictionary sort...";
        //dictionary.sort(eng::dictionary::less);
        std::sort(std::execution::par, dictionary.begin(),
        dictionary.end(), eng::dictionary::less);
        out << "dictionary sort ok";

        vocabulary.resize(dictionary.size());
        array<str> vocabulary_unicode;
        array<str> vocabulary_sorting;
        bool sort_out_next_line = false;

        for (int i=0; i<dictionary.size(); i++)
        {
            vocabulary[i] = eng::vocabulary::entry{};
            vocabulary[i].title =
            dictionary[i].title;

            bool unicode = false;
            bool sorting = false;
            for (char c : vocabulary[i].title)
            {
                auto b = static_cast<uint8_t>(c);
                if (b & 0b10000000) unicode = true; else
                if (c >= 'A') if (c < 'a' || 'z' < c) sorting = true;
            }
            if (unicode) sorting = true;
            if (unicode) vocabulary_unicode += vocabulary[i].title;
            if (sorting && !sort_out_next_line && i > 0) vocabulary_sorting += vocabulary[i-1].title;
            if (sorting || sort_out_next_line) vocabulary_sorting += vocabulary[i].title;
            sort_out_next_line = sorting;
        }
        {
            std::ofstream ofstream (dir / "vocabulary.txt");
            for (auto word : vocabulary)
                ofstream << word.title << "\n";
        }
        {
            std::ofstream ofstream (dir / "vocabulary_unicode.txt");
            for (auto word : vocabulary_unicode)
                ofstream << word << "\n";
        }
        {
            std::ofstream ofstream (dir / "vocabulary_sorting.txt");
            for (auto word : vocabulary_sorting)
                ofstream << word << "\n";
        }

        std::ofstream ofstream (dir / "vocabulary_problem.txt");

        for (int i=0; i<dictionary.size(); i++)
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
                redirect << "\n"; continue; }

            auto range = eng::vocabulary::find(redirect);
            if (range.empty()) {
                ofstream << "REDIRECT: NOT FOUND: " <<
                dictionary[i].title << " >>> " <<
                redirect << "\n"; continue; }

            auto n = range.offset();
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
                redirect << "\n"; continue; }

            dictionary[n].redirects += i;
            vocabulary[i].redirect = n;
        }

        dat::out::file dictionary_dat (dir / "dictionary.dat");

        for (int i=0; i<dictionary.size(); i++)
        {
            if ((i+1) % 100'000 == 0) out <<
                "written " + std::to_string
                    (i+1) + " entries";

            if (vocabulary[i].redirect >= 0) continue;

            vocabulary[i].offset = dictionary_dat.size;

            dictionary_dat << dictionary[i];
            
            vocabulary[i].length = dictionary_dat.size -
            vocabulary[i].offset;
        }

        ofstream = std::ofstream(dir / "vocabulary_redirect.txt");

        for (int i=0; i<dictionary.size(); i++)
        {
            if (vocabulary[i].redirect < 0) continue;

            int n = vocabulary[i].redirect;

            vocabulary[i].offset = vocabulary[n].offset;
            vocabulary[i].length = vocabulary[n].length;

            ofstream << 
            dictionary[i].title << " >>> " <<
            dictionary[n].title << "\n";
        }

        dat::out::file vocabulary_dat (dir / "vocabulary.dat");
        vocabulary_dat << 0x12345678; // endianness
        vocabulary_dat << vocabulary.size(); for (auto & entry : vocabulary)
        vocabulary_dat << entry;

        dictionary_dat.close();
        vocabulary_dat.close();

        array<eng::vocabulary::entry> Vocabulary;

        dat::in::pool pool;
        pool.bytes = dat::in::bytes (dir / "vocabulary.dat").value();
        dat::in::endianness = 0; // otherwise it would be reversed
        dat::in::endianness = pool.get_int();
        Vocabulary.resize(pool.get_int());
        for (auto & entry : Vocabulary)
            pool >> entry;

        ofstream = std::ofstream(dir / "vocabulary_check.txt");

        if (Vocabulary.size() !=
            vocabulary.size()) ofstream << "SIZE: " <<
            Vocabulary.size() << " != " <<
            vocabulary.size() << "\n";

        for (int i=0; i<vocabulary.size(); i++)
        {
            const auto & a = Vocabulary[i];
            const auto & b = vocabulary[i];
            if (a != b) ofstream <<
                a.title    << " [" <<
                a.offset   << ", " <<
                a.length   << ", " <<
                a.redirect << "] != " <<
                b.title    << " [" << 
                b.offset   << ", " << 
                b.length   << ", " << 
                b.redirect << "]\n";
        }

        ofstream = std::ofstream(dir / "vocabulary_numbers.txt");

        for (int i=0; i<vocabulary.size(); i++)
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
