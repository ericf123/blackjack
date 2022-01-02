#pragma once

#include "card.h"
#include <cstddef>

class HiLo {
public:
  HiLo(std::size_t decksPerShoe)
      : cardsPerShoe(decksPerShoe * bjcard::DECK_SIZE), rawCount(0),
        numCardsObserved(0) {}
  void addCard(const bjcard::Card& card);
  void reset();
  void setCardsPerShoe(std::size_t cardsPerShoe);
  int getRawCount() const;
  int getTrueCount() const;
  double getDecksRemaining() const;

private:
  std::size_t cardsPerShoe;
  int rawCount;
  std::size_t numCardsObserved;
};