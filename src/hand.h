#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "card.h"

using ConstCardIterator = std::vector<Card>::const_iterator;
using Wager = uint32_t;

class Hand {
public:
  static const CardTotal MAX_VALUE = 21;

  Hand() : Hand(0) {}
  Hand(Wager wager) : isSplit(false), isDoubled(false), wager(wager), value(0U) {}

  void reset();
  void addCard(Card card);
  Hand split();
  void doubleDown();

  ConstCardIterator getBeginIter() const;
  ConstCardIterator getEndIter() const;

  Wager getWager() const; 
  CardTotal getValue() const;

  bool canSplit() const;
  bool canDouble() const;
  bool isBlackjack() const;
  bool isSoft() const;
  bool isBusted() const;
private:
  bool isSplit;
  bool isDoubled;
  Wager wager;
  CardTotal value;
  std::vector<Card> cards;
};

static inline std::ostream& operator<<(std::ostream& os, Hand hand) {
  for (auto currCardIter = hand.getBeginIter(); currCardIter != hand.getEndIter(); ++currCardIter) {
    os << *currCardIter << " ";
  }
  return os;
}