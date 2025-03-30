#include "TextProcessing.h"

namespace TextProcessing
{
    // 1. Count words in text
    int countWords(const std::string& text)
    {
        std::istringstream stream(text);
        std::string word;
        int count = 0;

        while (stream >> word)
        {
            count++;
        }

        return count;
    }

    // 2. Count characters in text
    int countChars(const std::string& text)
    {
        return text.length();
    }

    // 3. Count lines in text
    int countLines(const std::string& text)
    {
        int count = 0;
        for (char c : text)
        {
            if (c == '\n')
            {
                count++;
            }
        }

        // Add 1 for the last line if text isn't empty and doesn't end with newline
        if (!text.empty() && text.back() != '\n')
        {
            count++;
        }

        return count;
    }

    // 4. Convert text to uppercase
    std::string toUppercase(const std::string& text)
    {
        std::string result = text;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c)
            { return std::toupper(c); });
        return result;
    }

    // 5. Convert text to lowercase
    std::string toLowercase(const std::string& text)
    {
        std::string result = text;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c)
            { return std::tolower(c); });
        return result;
    }

    // 6. Replace all occurrences of a word
    std::string replaceWord(const std::string& text, const std::string& target, const std::string& replacement)
    {
        if (target.empty())
            return text;

        std::string result = text;
        size_t pos = 0;

        while ((pos = result.find(target, pos)) != std::string::npos)
        {
            result.replace(pos, target.length(), replacement);
            pos += replacement.length();
        }

        return result;
    }

    // 7. Extract substring
    std::string extractSubstring(const std::string& text, int startPos, int length)
    {
        if (startPos < 0 || startPos >= static_cast<int>(text.length()))
        {
            return "";
        }

        // Adjust length if it would exceed the string's bounds
        if (length < 0 || startPos + length > static_cast<int>(text.length()))
        {
            length = text.length() - startPos;
        }

        return text.substr(startPos, length);
    }

    // 8. Reverse text
    std::string reverseText(const std::string& text)
    {
        std::string result = text;
        std::reverse(result.begin(), result.end());
        return result;
    }

    // 9. Remove punctuation
    std::string removePunctuation(const std::string& text)
    {
        std::string result;
        std::copy_if(text.begin(), text.end(), std::back_inserter(result),
            [](unsigned char c)
            { return !std::ispunct(c); });
        return result;
    }

    // 10. Count word frequency
    std::string wordFrequency(const std::string& text)
    {
        std::map<std::string, int> frequency;
        std::istringstream stream(text);
        std::string word;

        while (stream >> word)
        {
            // Remove punctuation from the word
            word.erase(std::remove_if(word.begin(), word.end(),
                [](unsigned char c)
                { return std::ispunct(c); }),
                word.end());

            // Convert to lowercase for case-insensitive comparison
            std::transform(word.begin(), word.end(), word.begin(),
                [](unsigned char c)
                { return std::tolower(c); });

            if (!word.empty())
            {
                frequency[word]++;
            }
        }

        // Format the result as a string
        std::ostringstream result;
        for (const auto& pair : frequency)
        {
            result << pair.first << ": " << pair.second << "\n";
        }

        return result.str();
    }

    // 11. Remove extra spaces
    std::string removeExtraSpaces(const std::string& text)
    {
        std::string result;
        bool lastWasSpace = false;

        for (char c : text)
        {
            if (std::isspace(c))
            {
                if (!lastWasSpace)
                {
                    result.push_back(' ');
                    lastWasSpace = true;
                }
            }
            else
            {
                result.push_back(c);
                lastWasSpace = false;
            }
        }

        // Trim leading and trailing spaces
        result.erase(0, result.find_first_not_of(' '));
        size_t lastNonSpace = result.find_last_not_of(' ');
        if (lastNonSpace != std::string::npos)
        {
            result.erase(lastNonSpace + 1);
        }

        return result;
    }

    // 12. Count sentences
    int countSentences(const std::string& text)
    {
        int count = 0;
        bool inSentence = false;

        for (char c : text)
        {
            if (std::isalnum(c))
            {
                inSentence = true;
            }
            else if ((c == '.' || c == '!' || c == '?') && inSentence)
            {
                count++;
                inSentence = false;
            }
        }

        // Count the last sentence if it doesn't end with a period
        if (inSentence)
        {
            count++;
        }

        return count;
    }
}