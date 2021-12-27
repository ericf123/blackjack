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
  static const CardTotal MAX_VALUE = 21;

  Hand() : Hand(0) {}
  Hand(Wager wager) : Hand(wager, 0) {}
  Hand(Wager wager, int depth)
      : depth(depth), hasChild(false), isDoubledDown(false), wager(wager),
        value(0U) {}

  void reset();
  void addCard(Card card);
  Hand split();
  void doubleDown();

  ConstCardIterator getBeginIter() const;
  ConstCardIterator getEndIter() const;

  Wager getWager() const;
  CardTotal getValue() const;
  int getDepth() const;

  bool canSplit() const;
  bool canDouble() const;
  bool isDoubled() const;
  bool isSplit() const;
  bool isSoft() const;
  bool isBlackjack() const;
  bool isBusted() const;

private:
  int depth;
  bool hasChild;
  bool isDoubledDown;
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