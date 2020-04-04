#pragma once
#include "app.h"
namespace app::dict
{
    array<eng::dictionary::entry> dictionary;
    array<eng::vocabulary::entry> vocabulary;

    auto less = [](const auto & a, const auto & b)
    { return eng::less(a.title, b.title); };

    auto less_case_insentive = [](const auto & a, const auto & b)
    { return eng::less_case_insentive(a.title, b.title); };

    auto vocabulary_range (str s)
    {
        auto entry = eng::vocabulary::entry{s};
        auto range = vocabulary.equal_range(entry, less); if (!range)
             range = vocabulary.equal_range(entry, less_case_insentive);
        return range;
    }

    array<str> lexical_items
    {
        "noun", "pronoun", "proper noun",
        "verb", "adjective", "adverb", "numeral", "number", "article", "particle", 
        "preposition", "postposition", "determiner", "conjunction", "interjection",
        "initialism", "contraction", "acronym", 
        "letter", "symbol", "punctuation mark", "diacritical mark",
        "suffix", "prefix", "infix", "affix", "interfix",
        "phrase", "prepositional phrase", "proverb", 
    };
    array<str> related_items
    {
        "abbreviations", "alternative forms", "synonyms", "antonyms",
        "hypernyms", "hyponyms", "meronyms", "holonyms", "troponyms",
        "derived terms", "related terms", "coordinate terms",
        "see also", 
    };
    array<str> lexical_notes
    {
        "pronunciation", "etymology",
        "usage notes", "trivia",
    };
}