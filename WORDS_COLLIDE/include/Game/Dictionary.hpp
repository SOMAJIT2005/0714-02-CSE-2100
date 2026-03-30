#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <string>
#include <vector>

class Dictionary {
public:
    void loadFromFile(const std::string& filepath);
    bool isValidWord(const std::string& word) const;
    bool isBonusWord(const std::string& word) const;

private:
    std::vector<std::string> loadedDictionary;
    const std::vector<std::string> specialBonusWords = {"BONUS","EXTRA","SPECIAL","MAGIC","EYE","GREEN"};
};

#endif // DICTIONARY_HPP