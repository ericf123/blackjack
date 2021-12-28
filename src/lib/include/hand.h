#pragma once

#include "card.h"
#include <cstdint>
#include <optional>
#include <vector>

using namespace bjcard;
using ConstCardIterator = std::vector<Card>::const_iterator;
using Wager = uint32_t;

class Hand {
public:
  static constexpr CardTotal MAX_VALUE = 21;

  Hand() : Hand(0) {}
  Hand(Wager wager)
      : isDoubledDown(false), isHandSplit(false), wager(wager), value(0U) {}

  void reset();
  void addCard(const Card& card);
  Hand split();
  void doubleDown();

  ConstCardIterator getBeginIter() const;
  ConstCardIterator getEndIter() const;

  Wager getWager() const;
  CardTotal getValue() const;

  bool canSplit() const;
  bool canDouble() const;
  bool isDoubled() const;
  bool isSplit() const;
  bool isSoft() const;
  bool isBlackjack() const;
  bool isBusted() const;

private:
  bool isDoubledDown;
  bool isHandSplit;
  Wager wager;
  CardTotal value;
  std::vector<Card> cards;
};

static inline std::ostream& operator<<(std::ostream& os, Hand hand) {
  for (auto currCardIter = hand.getBeginIter();
       currCardIter != hand.getEndIter(); ++currCardIter) {
    os << *currCardIter << " ";
  }
  return os;
}