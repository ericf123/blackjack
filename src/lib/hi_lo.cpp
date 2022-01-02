#include "hi_lo.h"
#include "card.h"
#include <cmath>

// 0 = Invalid
// 2-6 = +1
// 7-9 = 0
// 10-K, A = -1
static constexpr int COUNT_BY_RANK[14] = { 0, -1, 1, 1,  1,  1,  1,
                                           0, 0,  0, -1, -1, -1, -1 };

void HiLo::addCard(const bjcard::Card& card) {
  rawCount += COUNT_BY_RANK[static_cast<std::size_t>(card.rank)];
  ++numCardsObserved;
}

void HiLo::reset() {
  rawCount = 0;
  numCardsObserved = 0;
}

void HiLo::setCardsPerShoe(std::size_t cardsPerShoe) {
  this->cardsPerShoe = cardsPerShoe;
  reset();
}

int HiLo::getRawCount() const { return rawCount; }

int HiLo::getTrueCount() const {
  const auto decksRemaining = getDecksRemaining();
  if (decksRemaining > 0) {
    return std::round(rawCount / decksRemaining);
  }
  return 0;
}

double HiLo::getDecksRemaining() const {
  const auto cardsRemaining = cardsPerShoe - numCardsObserved;
  return static_cast<double>(cardsRemaining) / bjcard::DECK_SIZE;
}