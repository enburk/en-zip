#include "dat_out.h"
namespace studio::dic
{
    struct topic { str header, forms; array<str> content; };
    struct entry { str title; array<topic> topics; };
    inline array<entry> dictionary;
    inline array<str> vocabulary;

    bool wiktionary_update ()
    {
        using std::filesystem::path;
        path src = "../en-wiktionary/enwiktionary-data.txt";
        path dir = "../data";

        if (true)
        if (std::filesystem::exists         (dir / "vocabulary.dat") &&
            std::filesystem::last_write_time(dir / "vocabulary.dat") >
            std::filesystem::last_write_time(src))
            return false;

        std::filesystem::create_directories(dir);

        dictionary.reserve(1024*1024);

        str line, s1, s2;
        std::ifstream ifstream (src);
        while (std::getline(ifstream, line))
        {
            if (line == "") continue;
            if (line.starts_with("     == ")) {
                line.split_by   ("     == ", s1, s2);
                dictionary.back().topics += topic{}; s2.split_by(" == ",
                dictionary.back().topics.back().header,
                dictionary.back().topics.back().forms);
            }
            else
            if (line.starts_with("        # ")) {
                line.split_by   ("        # ", s1, s2);
                dictionary.back().topics.back().content += s2;
            }
            else dictionary += entry{line};
        }

        dictionary.shrink_to_fit();
        dictionary.sort([](auto & a, auto & b){ return eng::compare(a.title, b.title) < 0; });
        vocabulary.resize(dictionary.size());
        array<str> vocabulary_unicode;
        array<str> vocabulary_sorting;
        bool Sorting = false;

        for (int i=0; i<dictionary.size(); i++) {
            vocabulary[i] = dictionary[i].title;
            bool unicode = false, sorting = false;
            for (char c : vocabulary[i]) {
                if (static_cast<uint8_t>(c) & 0b10000000) unicode = true; else
                if (c >= 'A') if (c < 'a' || 'z' < c) sorting = true;
            }
            if (unicode) sorting = true;
            if (unicode) vocabulary_unicode += vocabulary[i];
            if (sorting && !Sorting && i > 0) vocabulary_sorting += vocabulary[i-1];
            if (sorting || Sorting) vocabulary_sorting += vocabulary[i];
            Sorting = sorting;
        }
        {
            std::ofstream ofstream (dir / "vocabulary.txt");
            for (auto word : vocabulary)
                ofstream << word << std::endl;
        }
        {
            std::ofstream ofstream (dir / "vocabulary_unicode.txt");
            for (auto word : vocabulary_unicode)
                ofstream << word << std::endl;
        }
        {
            std::ofstream ofstream (dir / "vocabulary_sorting.txt");
            for (auto word : vocabulary_sorting)
                ofstream << word << std::endl;
        }
        {
            dat::out::pool pool;
            pool << vocabulary;
            int unzip_size = pool.bytes.size();
            array<std::byte> zip = aux::zip(pool.bytes.whole()).value();
            pool.bytes.clear();
            pool << 0x12345678;
            pool << unzip_size;
            pool << zip;
            std::ofstream ofstream (dir / "vocabulary.dat", std::ios::binary);
            ofstream.write((char*)(pool.bytes.data()), pool.bytes.size());
        }

        return true;
    }
}
