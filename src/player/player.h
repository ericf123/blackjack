#pragma once

#include <vector>
#include "../hand.h"
#include "player_action.h"

using Bankroll = double;
using ConstHandIter = std::vector<Hand>::const_iterator;

class Player {
public:
  Player(Bankroll bankroll) : bankroll(bankroll), currHand(0) {}
  virtual void observeCard(Card card) = 0;
  virtual PlayerAction getNextAction() = 0;
  virtual Wager getWager() = 0;

  void receiveCard(Card card);
  void beginRound(Card firstCard, Wager wager);
  void endCurrentHand();
  void splitCurrentHand();
  void doubleCurrentHand();

  void credit(Wager winnings);
  void debit(Wager losses);

  ConstHandIter getBeginHand();
  ConstHandIter getEndHand();

  bool playingLastHand();
  CardTotal getCurrentHandValue();
  Wager getCurrentHandWager();
protected:
  std::vector<Hand> hands;
  size_t currHand;
  Bankroll bankroll;
};