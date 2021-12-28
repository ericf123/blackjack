#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <utility>

namespace bjcard {
static const std::string RANK_DISPLAY_STRS[] = { "?",  "A", "2", "3", "4",
                                                 "5",  "6", "7", "8", "9",
                                                 "10", "J", "Q", "K" };

// hearts, diamonds, spades, clubs
static const std::string SUIT_DISPLAY_STRS[] = { "\u2661", "\u2662", "\u2660",
                                                 "\u2663" };

using CardTotal = uint32_t;

static constexpr size_t DECK_SIZE = 52;

enum class Rank {
  Ace = 1,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Ten,
  Jack,
  Queen,
  King
};

enum class Suit { Hearts, Diamonds, Spades, Clubs };

struct Card {
  Rank rank;
  Suit suit;
};

// implement pre-increment for Rank, Suit only
template <typename T>
typename std::enable_if_t<std::is_enum_v<T>, T> operator++(T& t) {
  t = static_cast<T>(static_cast<size_t>(t) + 1);
  return t;
}

static inline std::ostream& operator<<(std::ostream& os, Rank rank) {
  os << RANK_DISPLAY_STRS[static_cast<size_t>(rank)];
  return os;
}

static inline std::ostream& operator<<(std::ostream& os, Suit suit) {
  os << SUIT_DISPLAY_STRS[static_cast<size_t>(suit)];
  return os;
}

static inline std::string getDisplayStr(const Card& card) {
  std::ostringstream oss;
  oss << card.rank << card.suit;
  return oss.str();
}

static inline std::ostream& operator<<(std::ostream& os, const Card& card) {
  const auto strToWrite = getDisplayStr(card);
  os.write(strToWrite.c_str(), strToWrite.size());
  return os;
}

template <typename ItType> CardTotal sumCards(ItType begin, ItType end) {
  auto total = 0U;
  auto numAces = 0;

  for (auto curr = begin; curr != end; ++curr) {
    const auto rank = curr->rank;
    total += std::min(static_cast<unsigned int>(rank), 10U);

    if (rank == Rank::Ace) {
      ++numAces;
    }
  }

  // can have at most one Ace with value of 11
  // by default Ace has value of 1
  if (numAces > 0 && total + 10 <= 21) {
    total += 10;
  }

  return total;
}

} // namespace bjcard