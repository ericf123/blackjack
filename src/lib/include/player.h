#pragma once

#include "event_router.h"
#include "hand.h"
#include "player_action.h"
#include <list>
#include <optional>

using Bankroll = double;
using ConstHandIter = std::list<Hand>::const_iterator;
using HandIter = std::list<Hand>::iterator;

class Player {
public:
  Player(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
         Bankroll bankroll);
  Bankroll getBankroll() const;

  std::optional<ConstHandIter> getBeginHand() const;
  std::optional<ConstHandIter> getEndHand() const;

  bool playingLastHand() const;
  CardTotal getCurrentHandValue() const;
  Wager getCurrentHandWager() const;
  std::optional<ConstHandIter> getCurrentHand() const;
  NodeId getNodeId() const;

protected:
  std::weak_ptr<EventRouter> router;
  OwningHandle sourceNode;
  Bankroll bankroll;
  std::optional<Card> dealerUpCard;
  std::list<Hand> hands;
  HandIter currHand;
};