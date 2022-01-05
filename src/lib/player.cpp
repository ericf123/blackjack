#include "player.h"
#include "blackjack_events.h"
#include "event_router.h"
#include <functional>
#include <memory>
#include <optional>

Player::Player(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
               Bankroll bankroll)
    : router(router), sourceNode(sourceNode), bankroll(bankroll),
      dealerUpCard(std::nullopt) {
  hands.push_back(Hand(0));
  currHand = hands.begin();
  if (!router.expired()) {
    auto r = router.lock();
    InvokeHandler<std::reference_wrapper<const Player>, ToConstRefInv<Player>>
        toConstRefInvHandler =
            [this](const WrappedEvent<ToConstRefInv<Player>>& e)
        -> std::reference_wrapper<const Player> {
      (void)e;
      return *this;
    };

    EventHandler<PlayerEndHandCmd> endHandler =
        [this](const WrappedEvent<PlayerEndHandCmd>& e) {
          (void)e;
          if (!playingLastHand()) {
            currHand = std::next(currHand);
          }
        };

    EventHandler<PlayerDoubleHandCmd> doubleHandler =
        [this](const WrappedEvent<PlayerDoubleHandCmd>& e) {
          (void)e;
          currHand->doubleDown();
        };

    EventHandler<PlayerSplitHandCmd> splitHandler =
        [this](const WrappedEvent<PlayerSplitHandCmd>& e) {
          (void)e;
          hands.push_back(currHand->split());
        };

    EventHandler<CardResp> receiveCardHandler =
        [this](const WrappedEvent<CardResp>& e) {
          currHand->addCard(e.event.card);
        };

    EventHandler<AdjustBankrollCmd> bankrollAdjustmentHandler =
        [this](const WrappedEvent<AdjustBankrollCmd>& e) {
          this->bankroll += e.event.changeAmount;
        };

    EventHandler<PlayerReceiveUpCardCmd> receiveUpCardHandler =
        [this](const WrappedEvent<PlayerReceiveUpCardCmd>& e) {
          dealerUpCard.emplace(e.event.card);
        };

    EventHandler<PlayerStartRoundCmd> startRoundHandler =
        [this](const WrappedEvent<PlayerStartRoundCmd>& e) {
          hands.clear();

          hands.push_front(Hand(e.event.wager));
          currHand = hands.begin();
        };

    r->registerInvokeHandler(sourceNode, toConstRefInvHandler);
    r->listen(sourceNode, false, receiveCardHandler);
    r->listen(sourceNode, false, endHandler);
    r->listen(sourceNode, false, doubleHandler);
    r->listen(sourceNode, false, splitHandler);
    r->listen(sourceNode, false, bankrollAdjustmentHandler);
    r->listen(sourceNode, false, receiveUpCardHandler);
    r->listen(sourceNode, false, startRoundHandler);
  }
}

NodeId Player::getNodeId() const { return *sourceNode; }
Bankroll Player::getBankroll() const { return bankroll; }

bool Player::playingLastHand() const {
  return std::next(currHand) == hands.end();
}

CardTotal Player::getCurrentHandValue() const {
  if (currHand != hands.end()) {
    return currHand->getValue();
  }

  return 0;
}

Wager Player::getCurrentHandWager() const {
  if (currHand != hands.end()) {
    return currHand->getWager();
  }

  return 0;
}

std::optional<ConstHandIter> Player::getCurrentHand() const {
  if (!hands.empty()) {
    return currHand;
  }

  return std::nullopt;
}

std::optional<ConstHandIter> Player::getBeginHand() const {
  if (!hands.empty()) {
    return hands.cbegin();
  }

  return std::nullopt;
}

std::optional<ConstHandIter> Player::getEndHand() const {
  if (!hands.empty()) {
    return hands.cend();
  }

  return std::nullopt;
}