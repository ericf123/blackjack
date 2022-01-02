#pragma once
#include <ncurses.h>

namespace bjcolor {
constexpr chtype BKGD_GREEN = 255;
constexpr chtype CARD_RED = 254;
constexpr chtype PAIR_BKGD = 1;
constexpr chtype PAIR_BKGD_INV = 2;
constexpr chtype PAIR_CARD_BLACK = 3;
constexpr chtype PAIR_CARD_RED = 4;
constexpr chtype PAIR_CARD_DOWN = 5;
} // namespace bjcolor

namespace bjdim {
constexpr int CARD_HEIGHT = 7;
constexpr int CARD_WIDTH = 5;
constexpr int HAND_HEIGHT = 2 * CARD_HEIGHT + 1 + 4;
constexpr int HAND_WIDTH = 4 * CARD_WIDTH + 3 + 2;
constexpr int STATS_WIDTH = 25;
constexpr int TITLE_HEIGHT = 1;
constexpr int TABLE_HEIGHT = HAND_HEIGHT * 2 + 4 + 2;
constexpr int WAGER_INPUT_WIDTH = 7;
constexpr int WAGER_WIDTH = WAGER_INPUT_WIDTH + sizeof("Wager: $") + 1;
constexpr int WAGER_HEIGHT = 5;
} // namespace bjdim
