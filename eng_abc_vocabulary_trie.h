#pragma once
#include "eng_abc_vocabulary.h"
namespace eng
{
    using vocabulary_trie = vocabulary_cached;

    struct vocabulary_triee
    {
        struct node
        {
            str letter;
            array<node> letters;
            int offset = 0;
            int length = 0;
            int redirect = -1;
            int total = 0;
        };
        node root;

        int size () const { return root.total; }
        node& operator [] (int i) { return root; }

        std::optional<int> index (str const& s)
        {
            return 0;
        }

        int lower_bound_case_insensitive (str const& s)
        {
            return 0;
        }

        int lower_bound (str const& s)
        {
            return 0;
        }

        explicit vocabulary_triee () = default;
        explicit vocabulary_triee (dictionary& dic)
        {
            struct heapnode
            {
                array<heapnode> letters;
                int total;
                bool final = false;
                bool ligature = false;
            };
            heapnode root;
            //for (auto& e: dictionary)
            //    root.add(e);


        }
        explicit vocabulary_triee (std::filesystem::path path)
        {
        }
        void save (sys::out::file& file)
        {
        }
    };
}
