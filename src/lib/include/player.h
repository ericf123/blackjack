#pragma once

#include <optional>
#include <vector>
#include "hand.h"
#include "player_action.h"

using Bankroll = double;
using ConstHandIter = std::vector<Hand>::const_iterator;

class Player {
public:
  Player(Bankroll bankroll) : bankroll(bankroll), currHand(0), dealerUpCard(std::nullopt) {}
  virtual void observeCard(Card card) = 0;
  virtual PlayerAction getNextAction() = 0;
  virtual Wager getWager() = 0;

  virtual void beginRound(Card firstCard, Wager wager);
  virtual void receiveCard(Card card);
  virtual void setDealerUpCard(Card card);

  virtual void splitCurrentHand();
  virtual void doubleCurrentHand();
  virtual void endCurrentHand();

  virtual void credit(Wager winnings);
  virtual void debit(Wager losses);

  Bankroll getBankroll();

  ConstHandIter getBeginHand();
  ConstHandIter getEndHand();

  bool playingLastHand();
  CardTotal getCurrentHandValue();
  Wager getCurrentHandWager();
  const Hand* getCurrentHand();
protected:
  std::vector<Hand> hands;
  std::optional<Card> dealerUpCard;
  size_t currHand;
  Bankroll bankroll;
};