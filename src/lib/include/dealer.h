#pragma once

#include "card.h"
#include "table.h"
#include <memory>

using namespace bjcard;

class Dealer {
public:
  Dealer(std::weak_ptr<EventRouter> router, OwningHandle sourceNode);

public:
  ConstPlayerNodeIter getCurrPlayerNode() const;
  bool checkDealerBlackjack() const;
  NodeId getNodeId() const;
  ConstHandIter getDealerHand() const;

private:
  const CardTotal DEALER_STAND_VALUE = 17;
  HandIter dealerHand;
  std::list<Hand> dealerHands;
  OwningHandle sourceNode;
  ConstPlayerNodeIter currPlayer;
  ConstPlayerNodeIter endPlayer;
  std::weak_ptr<EventRouter> router;

  bool shouldPlayDealerHand();
  bool handlePlayerAction(Player& player, PlayerAction action);

  void dealInitialCards();
  void playDealerHand();
  void handleRoundResults();
};