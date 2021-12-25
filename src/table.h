#pragma once

#include <optional>
#include <memory> 
#include <vector>
#include "deck.h"
#include "player/player.h"


using PlayerPtrIter = std::vector<std::unique_ptr<Player>>::iterator;

class Table {
public:
  Table(size_t numDecks, double blackjackPayoutRatio, bool dealerHitSoft17);

  bool shouldDealerHitSoft17() const { return dealerHitSoft17; }
  bool getBlackjackPayoutRatio() const { return blackjackPayoutRatio; }

  PlayerPtrIter getBeginPlayer();
  PlayerPtrIter getEndPlayer();

  void addPlayer(std::unique_ptr<Player> player);
  Card drawCard();
  void shuffleIfNeeded();
  void forceShuffle();
private:
  // estimate of maximum number of cards a player can take per round
  // this is used to determine whether or not the deck needs to be shuffled
  // prior to starting a round
  const size_t EST_MAX_PLAYER_CARDS_PER_ROUND = 6;

  const double blackjackPayoutRatio;
  const bool dealerHitSoft17;
  bool firstRound;
  Deck deck;
  std::vector<std::unique_ptr<Player>> players;
};