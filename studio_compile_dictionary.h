#pragma once
#include "dat_out.h"
#include "app_dict.h"
namespace studio::compile
{
    bool dictionary_update (gui::text::console & out, gui::text::console & err)
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

        dictionary.sort(eng::dictionary::less);
        vocabulary.resize(dictionary.size());
        array<str> vocabulary_unicode;
        array<str> vocabulary_sorting;
        bool sort_out_next_line = false;

        out << "dictionary sort ok";

        for (int i=0; i<dictionary.size(); i++)
        {
            vocabulary[i].title = dictionary[i].title;
            bool unicode = false, sorting = false;
            for (char c : vocabulary[i].title) {
                if (static_cast<uint8_t>(c) & 0b10000000) unicode = true; else
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

        dat::out::file dictionary_dat (dir / "dictionary.dat");

        for (int i=0; i<dictionary.size(); i++)
        {
            if ((i+1) % 100'000 == 0) out << "wrote " + std::to_string(i+1) + " entries";

            auto & topics = dictionary[i].topics;

            if (topics.size() == 1 && topics[0].header == ">>>") continue;

            vocabulary[i].offset = dictionary_dat.size;
            
            dictionary_dat << dictionary[i];
            
            vocabulary[i].length = dictionary_dat.size - vocabulary[i].offset;
        }

        std::ofstream ofstream (dir / "vocabulary_problem.txt");

        for (int i=0; i<dictionary.size(); i++)
        {
            auto & topics = dictionary[i].topics;

            if (topics.size() == 1 && topics[0].header == ">>>")
            {
                str redirect = topics[0].forms;
                if (redirect == vocabulary[i].title) { ofstream << "REDIRECT: ITSELF: " <<
                    dictionary[i].title << " >>> " << redirect << std::endl; continue; }

                auto it = vocabulary.lower_bound(eng::vocabulary::entry{redirect}, eng::vocabulary::less);
                if (it == vocabulary.end()) { ofstream << "REDIRECT: NOT FOUND: " <<
                    dictionary[i].title << " >>> " << redirect << std::endl; continue; }

                auto n = (int)(it - vocabulary.begin());
                if (vocabulary[n].length == 0) { ofstream << "REDIRECT: TRANSIT: " <<
                    dictionary[i].title << " >>> " << redirect << std::endl; continue; }

                vocabulary[i].offset = vocabulary[n].offset;
                vocabulary[i].length = vocabulary[n].length;
            }
        }

        dat::out::file vocabulary_dat (dir / "vocabulary.dat");
        vocabulary_dat << 0x12345678; // endianness
        vocabulary_dat << vocabulary.size(); for (auto & entry : vocabulary)
        vocabulary_dat << entry;

        out << "dictionary update ok";
        out << "";

        return true;
    }
}
