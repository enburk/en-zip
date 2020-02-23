#include "dat.h"
namespace studio::dic
{
    bool wiktionary_update ()
    {
        using namespace std::filesystem;
        path src = "../en-wiktionary/enwiktionary-data.txt";
        path dir = "../data";

        if (dir != std::filesystem::path())
            if (!std::filesystem::create_directories(dir))
                throw std::runtime_error("couldn't create dir: " + dir.string());

        eng::dictionary.reserve(1024*1024);

        str line, s1, s2;
        std::ifstream ifstream (src);
        while (std::getline(ifstream, line))
        {
            if (line == "") continue;
            if (line.starts_with("     == ")) {
                line.split_by   ("     == ", s1, s2);
                eng::dictionary.back().topics += eng::topic{};
                s2.split_by(" == ",
                    eng::dictionary.back().topics.back().header,
                    eng::dictionary.back().topics.back().forms);
            }
            else
            if (line.starts_with("        # ")) {
                line.split_by   ("        # ", s1, s2);
                eng::dictionary.back().topics.back().content += s2;
            }
            else {
                eng::dictionary += eng::entry{line};
            }
        }

        eng::dictionary.shrink_to_fit();

        std::ofstream ofstream (dir / "vocabulary.txt");
        for (auto word : eng::vocabulary)
            ofstream << word << std::endl;

        return true;
    }
}
