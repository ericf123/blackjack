#pragma once

#include "event_router.h"
#include "player.h"
#include "shoe.h"
#include <list>
#include <memory>
#include <optional>

using PlayerNodeIter = std::list<NodeId>::iterator;
using ConstPlayerNodeIter = std::list<NodeId>::const_iterator;

class Table {
public:
  Table(size_t numDecks, size_t minCardsInShoe, double blackjackPayoutRatio,
        bool dealerHitSoft17);

  bool shouldDealerHitSoft17() const { return dealerHitSoft17; }
  double getBlackjackPayoutRatio() const { return blackjackPayoutRatio; }

  OwningHandle registerPlayer();
  std::weak_ptr<EventRouter> getRouter();
  NodeId getNodeId() const;

  ConstPlayerNodeIter getBeginPlayer() const;
  ConstPlayerNodeIter getEndPlayer() const;

private:
  const double blackjackPayoutRatio;
  const bool dealerHitSoft17;
  bool firstRound;
  Shoe shoe;
  std::list<NodeId> players;
  std::shared_ptr<EventRouter> router;
  OwningHandle sourceNode;
};