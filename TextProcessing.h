#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <map>
#include <regex>

// Text processing functions
namespace TextProcessing
{
    // Function codes to identify operations
    enum FunctionCode
    {
        COUNT_WORDS = 1,
        COUNT_CHARS = 2,
        COUNT_LINES = 3,
        TO_UPPERCASE = 4,
        TO_LOWERCASE = 5,
        REPLACE_WORD = 6,
        EXTRACT_SUBSTRING = 7,
        REVERSE_TEXT = 8,
        REMOVE_PUNCTUATION = 9,
        WORD_FREQUENCY = 10,
        REMOVE_EXTRA_SPACES = 11,
        COUNT_SENTENCES = 12
    };

    // 1. Count words in text
    int countWords(const std::string& text);

    // 2. Count characters in text
    int countChars(const std::string& text);

    // 3. Count lines in text
    int countLines(const std::string& text);

    // 4. Convert text to uppercase
    std::string toUppercase(const std::string& text);

    // 5. Convert text to lowercase
    std::string toLowercase(const std::string& text);

    // 6. Replace all occurrences of a word
    std::string replaceWord(const std::string& text, const std::string& target, const std::string& replacement);

    // 7. Extract substring
    std::string extractSubstring(const std::string& text, int startPos, int length);

    // 8. Reverse text
    std::string reverseText(const std::string& text);

    // 9. Remove punctuation
    std::string removePunctuation(const std::string& text);

    // 10. Count word frequency
    std::string wordFrequency(const std::string& text);

    // 11. Remove extra spaces
    std::string removeExtraSpaces(const std::string& text);

    // 12. Count sentences
    int countSentences(const std::string& text);
}