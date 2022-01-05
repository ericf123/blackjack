#pragma once

#include "card.h"
#include "table.h"
#include <memory>

using namespace bjcard;

class Dealer {
public:
  Dealer(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
         const NodeId& tableNode);

public:
  ConstPlayerNodeIter getCurrPlayerNode() const;
  bool checkDealerBlackjack() const;
  NodeId getNodeId() const;
  ConstHandIter getDealerHand() const;
  // TODO: use events instead
  void handleRoundResults();

private:
  const CardTotal DEALER_STAND_VALUE = 17;
  HandIter dealerHand;
  std::list<Hand> dealerHands;
  OwningHandle sourceNode;
  NodeId tableNode;
  ConstPlayerNodeIter beginPlayer;
  ConstPlayerNodeIter currPlayer;
  ConstPlayerNodeIter endPlayer;
  std::weak_ptr<EventRouter> router;

  void publishUpCard(const Card& card);
  bool shouldPlayDealerHand();
  bool handlePlayerAction(Player& player, PlayerAction action);

  void dealInitialCards();
  void playDealerHand();
};