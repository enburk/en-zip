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
        if (std::filesystem::exists         (dir / "dictionary_entries.dat") and
            std::filesystem::exists         (dir / "dictionary_indices.dat") and
            std::filesystem::exists         (dir / "vocabulary.dat") and
            std::filesystem::last_write_time(dir / "vocabulary.dat") >
            std::filesystem::last_write_time(src))
            return false;

        out << "dictionary update...";

        std::filesystem::create_directories(dir);

        eng::dictionary dictionary;
        auto& entries = dictionary.entries;
        auto& indices = dictionary.indices;
        entries.reserve(1024*1024);
        indices.reserve(1024*1024);

        str line, s1, s2;
        std::ofstream ofstream;
        std::ifstream ifstream (src);
        while (std::getline(ifstream, line))
        {
            if (line == "") continue;
            if (line.starts_with("  == ")) {
                line.split_by   ("  == ", s1, line);
                line.split_by   ( " == ", s1, s2);
                entries.back().topics += eng::dictionary::topic{};
                entries.back().topics.back().header = s1;
                entries.back().topics.back().forms = s2;
            }
            else
            if (line.starts_with("     # ")) {
                line.split_by   ("     # ", s1, s2);
                entries.back().topics.back().content += s2;
            }
            else
            {
                indices += eng::dictionary::index{};
                entries += eng::dictionary::entry{line};
                if (entries.size() % 100'000 == 0)
                    out << "read " + std::to_string(
                    entries.size()) + " entries";
            }
        }

        out << "dictionary sort...";
        std::sort(std::execution::par_unseq,
            entries.begin(), entries.end(),
            [](auto const& a, auto const& b) { return
            eng::less(a.title, b.title); });
        out << "dictionary sort ok";

        out << "vocabulary save...";
        ofstream = std::ofstream(dir/"vocabulary.txt");
        for (auto& e: entries) ofstream << e.title << "\n";

        eng::vocabulary vocabulary(dictionary);
        dat::out::file vocabulary_out(dir/"vocabulary.dat");
        vocabulary.save(vocabulary_out);
        vocabulary_out.close();

        out << "vocabulary check...";
        vocabulary = std::move(eng::vocabulary(dir/"vocabulary.dat"));
        ofstream = std::ofstream(dir/"x_errors_vocabulary.txt");

        if (vocabulary.size() !=
            entries   .size()) ofstream << "SIZE: " <<
            vocabulary.size() << " != " <<
            entries   .size() << "\n";

        for (int i=0; i<vocabulary.size(); i++)
        {
            const auto & a = entries[i];
            const auto & b = vocabulary[i];
            if (a.title != b.title) ofstream << i << ": " <<
                a.title << " != " <<
                b.title << "\n";
        }

        out << "dictionary check...";
        ofstream = std::ofstream(dir/"x_errors_dictionary.txt");

        for (int i=0; i<entries.size(); i++)
        {
            auto & topics =
            entries[i].topics;
            if (topics.size() != 1 or
                topics.front().header != ">>>")
                continue; // it's not a redirect

            str redirect = topics[0].forms;
            if (redirect == entries[i].title) {
                ofstream << "REDIRECT: TO ITSELF: " <<
                entries[i].title << " >>> " <<
                redirect << "\n";
                continue; }

            auto index = vocabulary.index(redirect);
            if (not index) {
                ofstream << "REDIRECT: NOT FOUND: " <<
                entries[i].title << " >>> " <<
                redirect << "\n";
                continue; }

            int n = *index;
            if (entries[n].title != redirect)
                ofstream << "REDIRECT: MISMATCH: " <<
                entries[i].title << " >>> " <<
                entries[n].title << " <<< " <<
                redirect << "\n";

            auto & topics2 =
            entries[n].topics;
            if (topics2.size() == 1 and
                topics2.front().header == ">>>") {
                ofstream << "REDIRECT: TRANSIT: " <<
                entries[i].title << " >>> " <<
                redirect << "\n";
                continue; }

            entries[n].redirects += i;
            indices[i].redirect = n;
        }

        out << "dictionary entries save...";
        dat::out::file entries_out(dir/"dictionary_entries.dat");
        dat::out::file indices_out(dir/"dictionary_indices.dat");

        for (int i=0; i<entries.size(); i++)
        {
            if ((i+1) % 100'000 == 0) out <<
                "written " + std::to_string
                    (i+1) + " entries";

            if ( // skip redirected
            indices[i].redirect >= 0) continue;
            indices[i].offset = entries_out.size;
            entries[i] >>       entries_out;
            indices[i].length = entries_out.size -
            indices[i].offset;
        }

        out << "dictionary indices save...";
        ofstream = std::ofstream(dir/"x_redirects.txt");

        for (int i=0; i<indices.size(); i++)
        {
            int n = indices[i].redirect;
            if (n < 0) continue; // ony for redirected

            indices[i].offset = indices[n].offset;
            indices[i].length = indices[n].length;

            ofstream << 
            entries[i].title << " >>> " <<
            entries[n].title << "\n";
        }

        ofstream = std::ofstream(dir/"x_numbers.txt");

        for (int i=0; i<indices.size(); i++)
        {
            indices[i] >> indices_out;

            ofstream  << i << " "  <<
                entries[i].title    << " [" <<
                indices[i].offset   << ", " <<
                indices[i].length   << ", " <<
                indices[i].redirect << "]\n";
        }

        entries_out.close();
        indices_out.close();

        out << "dictionary update ok";
        out << "";

        return true;
    }
}
