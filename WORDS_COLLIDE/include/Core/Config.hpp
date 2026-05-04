#ifndef CONFIG_HPP
#define CONFIG_HPP

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;
constexpr int GRID_SIZE = 20;
constexpr int TILE_SIZE = WINDOW_WIDTH / GRID_SIZE;
constexpr int MAX_WORD_LENGTH = 20;
constexpr int MAX_SCORED_WORDS = 50;
constexpr int MAX_NAME_LENGTH = 30;
constexpr unsigned int GAME_DURATION_MS = 600000;
constexpr unsigned int WARNING_TIME_MS = 50000;
constexpr unsigned int WARNING_INTERVAL_MS = 10000;
constexpr unsigned int TURN_DURATION_MS = 20000;
constexpr int BONUS_WORD_POINTS = 15;
constexpr int MAX_BONUS_WORDS = 50;
constexpr int MAX_WORDS_IN_DICT = 200000;

#endif // CONFIG_HPP