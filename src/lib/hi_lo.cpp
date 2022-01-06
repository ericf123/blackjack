#include "hi_lo.h"
#include "blackjack_events.h"
#include "card.h"
#include <cmath>

// 0 = Invalid
// 2-6 = +1
// 7-9 = 0
// 10-K, A = -1
static constexpr int COUNT_BY_RANK[14] = { 0, -1, 1, 1,  1,  1,  1,
                                           0, 0,  0, -1, -1, -1, -1 };

HiLo::HiLo(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
           std::size_t decksPerShoe)
    : router(router), sourceNode(sourceNode),
      cardsPerShoe(decksPerShoe * bjcard::DECK_SIZE), rawCount(0),
      numCardsObserved(0) {
  if (auto r = router.lock()) {
    EventHandler<void, CardResp> cardHandler =
        [this](const WrappedEvent<void, CardResp>& e) {
          addCard(e.event.card);
          e.router.broadcast(this->sourceNode,
                             CountNotification<void>{ getRawCount(),
                                                      getTrueCount(),
                                                      getDecksRemaining() });
        };

    EventHandler<void, ShuffleNotification> shuffleHandler =
        [this](const WrappedEvent<void, ShuffleNotification>& e) {
          (void)e;
          reset();
        };

    r->listen(sourceNode, true, cardHandler);
    r->listen(sourceNode, true, shuffleHandler);
  }
}

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