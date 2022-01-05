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

  EventHandler<CardReq> cardRequestHandler =
      [this](const WrappedEvent<CardReq>& e) {
        e.router.send(sourceNode, e.event.receiver, CardResp{ shoe.draw() },
                      e.event.secret);
      };
  EventHandler<ShuffleIfNeededCmd> shuffleIfNeededCmdHandler =
      [this](const WrappedEvent<ShuffleIfNeededCmd>& e) {
        (void)e;

        if (firstRound || shoe.needsShuffle()) {
          shoe.shuffle();
          firstRound = false;
        }
      };
  EventHandler<TableBeginPlayerReq> beginPlayerHandler =
      [this](const WrappedEvent<TableBeginPlayerReq>& e) {
        e.router.send(sourceNode, e.sourceId,
                      TableBeginPlayerResp{ players.begin() });
      };
  EventHandler<TableEndPlayerReq> endPlayerHandler =
      [this](const WrappedEvent<TableEndPlayerReq>& e) {
        e.router.send(sourceNode, e.sourceId,
                      TableEndPlayerResp{ players.end() });
      };

  InvokeHandler<std::reference_wrapper<const Table>, ToConstRefInv<Table>>
      toConstRefInvHandler =
          [this](const WrappedEvent<ToConstRefInv<Table>>& e) -> const Table& {
    (void)e;
    return *this;
  };

  router->listen(sourceNode, false, cardRequestHandler);
  router->listen(sourceNode, false, shuffleIfNeededCmdHandler);
  router->listen(sourceNode, false, beginPlayerHandler);
  router->listen(sourceNode, false, endPlayerHandler);
  router->registerInvokeHandler(sourceNode, toConstRefInvHandler);
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
