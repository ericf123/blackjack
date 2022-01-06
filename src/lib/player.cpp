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
  if (auto r = router.lock()) {
    EventHandler<std::reference_wrapper<const Player>, ToConstRefInv>
        toConstRefInvHandler =
            [this](const WrappedEvent<std::reference_wrapper<const Player>,
                                      ToConstRefInv>& e)
        -> std::reference_wrapper<const Player> {
      (void)e;
      return *this;
    };

    EventHandler<void, PlayerEndHandCmd> endHandler =
        [this](const WrappedEvent<void, PlayerEndHandCmd>& e) {
          (void)e;
          if (!playingLastHand()) {
            currHand = std::next(currHand);
          }
        };

    EventHandler<void, PlayerDoubleHandCmd> doubleHandler =
        [this](const WrappedEvent<void, PlayerDoubleHandCmd>& e) {
          (void)e;
          currHand->doubleDown();
        };

    EventHandler<void, PlayerSplitHandCmd> splitHandler =
        [this](const WrappedEvent<void, PlayerSplitHandCmd>& e) {
          (void)e;
          hands.push_back(currHand->split());
        };

    EventHandler<void, CardResp> receiveCardHandler =
        [this](const WrappedEvent<void, CardResp>& e) {
          currHand->addCard(e.event.card);
        };

    EventHandler<void, AdjustBankrollCmd> bankrollAdjustmentHandler =
        [this](const WrappedEvent<void, AdjustBankrollCmd>& e) {
          this->bankroll += e.event.changeAmount;
        };

    EventHandler<void, PlayerReceiveUpCardCmd> receiveUpCardHandler =
        [this](const WrappedEvent<void, PlayerReceiveUpCardCmd>& e) {
          dealerUpCard.emplace(e.event.card);
        };

    EventHandler<void, PlayerStartRoundCmd> startRoundHandler =
        [this](const WrappedEvent<void, PlayerStartRoundCmd>& e) {
          hands.clear();

          hands.push_front(Hand(e.event.wager));
          currHand = hands.begin();
        };

    r->listen(sourceNode, false, toConstRefInvHandler);
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