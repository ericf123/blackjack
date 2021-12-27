#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>

using CardTotal = uint32_t;

enum class Card {
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

static const std::string DISPLAY_STRS[] = {
  "?", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
};

static inline std::string getDisplayStr(Card card) {
  return DISPLAY_STRS[static_cast<size_t>(card)];
}

static inline std::ostream& operator<<(std::ostream& os, Card card) {
  os << getDisplayStr(card);
  return os;
}

template <typename ItType> CardTotal sumCards(ItType begin, ItType end) {
  auto total = 0U;
  auto numAces = 0;

  for (auto curr = begin; curr != end; ++curr) {
    auto cardEnumValue = static_cast<CardTotal>(*curr);
    total += std::min(cardEnumValue, 10U);

    if (*curr == Card::Ace) {
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