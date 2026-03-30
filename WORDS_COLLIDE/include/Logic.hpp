#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "Types.hpp"
#include <string>

class Logic {
public:
    static void loadDictionary();
    static bool isValidWord(const std::string& word);
    static int checkAndScore(GameState& game, int x, int y, char letter);
};

#endif // LOGIC_HPP
