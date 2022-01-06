#include "table.h"
#include "blackjack_events.h"
#include "event_router.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>

Table::Table(size_t numDecks, size_t minCardsInShoe,
             double blackjackPayoutRatio, bool dealerHitSoft17)
    : blackjackPayoutRatio(blackjackPayoutRatio),
      dealerHitSoft17(dealerHitSoft17), firstRound(true),
      shoe(numDecks, minCardsInShoe), router(std::make_shared<EventRouter>()) {
  sourceNode = router->requestId();

  EventHandler<void, CardReq> cardRequestHandler =
      [this](const WrappedEvent<void, CardReq>& e) {
        e.router.invoke(sourceNode, e.event.receiver,
                        CardResp<void>{ shoe.draw() }, e.event.secret);
      };

  EventHandler<void, ShuffleIfNeededCmd> shuffleIfNeededCmdHandler =
      [this](const WrappedEvent<void, ShuffleIfNeededCmd>& e) {
        (void)e;

        if (firstRound || shoe.needsShuffle()) {
          shoe.shuffle();
          firstRound = false;
        }
      };

  EventHandler<std::reference_wrapper<const Table>, ToConstRefInv>
      toConstRefInvHandler =
          [this](const WrappedEvent<std::reference_wrapper<const Table>,
                                    ToConstRefInv>& e) -> const Table& {
    (void)e;
    return *this;
  };

  router->listen(sourceNode, false, cardRequestHandler);
  router->listen(sourceNode, false, shuffleIfNeededCmdHandler);
  router->listen(sourceNode, false, toConstRefInvHandler);
}

OwningHandle Table::registerPlayer() {
  auto handle = router->requestId();
  players.push_back(*handle);
  return handle;
}

std::weak_ptr<EventRouter> Table::getRouter() { return router; }

NodeId Table::getNodeId() const { return *sourceNode; }

ConstPlayerNodeIter Table::getBeginPlayer() const { return players.cbegin(); };

ConstPlayerNodeIter Table::getEndPlayer() const { return players.cend(); };
