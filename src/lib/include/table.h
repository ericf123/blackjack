#pragma once

#include "player.h"
#include "shoe.h"
#include <memory>
#include <optional>
#include <vector>

using PlayerPtrIter = std::vector<std::shared_ptr<Player>>::iterator;

class Table {
public:
  Table(size_t numDecks, size_t minCardsInShoe, double blackjackPayoutRatio,
        bool dealerHitSoft17);

  bool shouldDealerHitSoft17() const { return dealerHitSoft17; }
  double getBlackjackPayoutRatio() const { return blackjackPayoutRatio; }

  PlayerPtrIter getBeginPlayer();
  PlayerPtrIter getEndPlayer();

  void addPlayer(std::shared_ptr<Player> player);
  void showCardToPlayers(Card card);
  Card drawCard();
  Card drawCard(bool observable);
  void shuffleIfNeeded();
  void forceShuffle();

private:
  const double blackjackPayoutRatio;
  const bool dealerHitSoft17;
  bool firstRound;
  Shoe shoe;
  std::vector<std::shared_ptr<Player>> players;
};