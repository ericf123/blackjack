#pragma once

#include "card.h"
#include "event_def.h"
#include <cstddef>

class HiLo {
public:
  HiLo(std::weak_ptr<EventRouter>, OwningHandle sourceNode,
       std::size_t decksPerShoe);
  void addCard(const bjcard::Card& card);
  void reset();
  void setCardsPerShoe(std::size_t cardsPerShoe);
  int getRawCount() const;
  int getTrueCount() const;
  double getDecksRemaining() const;

private:
  std::weak_ptr<EventRouter> router;
  OwningHandle sourceNode;

  std::size_t cardsPerShoe;
  int rawCount;
  std::size_t numCardsObserved;
};