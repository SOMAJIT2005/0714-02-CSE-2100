#include "Game/Dictionary.hpp"
#include "Core/Config.hpp"
#include <cstdio>
#include <algorithm>
#include <cctype>
#include <iostream>

void Dictionary::loadFromFile(const std::string& filepath) {
    loadedDictionary.clear();
    FILE* f = std::fopen(filepath.c_str(), "r");
    if (!f) {
        std::cerr << "FATAL ERROR: " << filepath << " not found!\n";
        return;
    }
    char buf[256];
    while (std::fscanf(f, "%255s", buf) == 1 && loadedDictionary.size() < static_cast<size_t>(MAX_WORDS_IN_DICT)) {
        std::string s(buf);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        loadedDictionary.push_back(s);
    }
    std::fclose(f);
    std::sort(loadedDictionary.begin(), loadedDictionary.end());
    std::cout << "Loaded " << loadedDictionary.size() << " words into memory.\n";
}

bool Dictionary::isValidWord(const std::string& word) const {
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return std::binary_search(loadedDictionary.begin(), loadedDictionary.end(), up);
}

bool Dictionary::isBonusWord(const std::string& word) const {
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    for (const auto &b : specialBonusWords) {
        if (up == b) return true;
    }
    return false;
}