#pragma once

#include "hand.h"
#include "player_action.h"
#include <list>
#include <optional>

using Bankroll = double;
using ConstHandIter = std::list<Hand>::const_iterator;
using HandIter = std::list<Hand>::iterator;

class Player {
public:
  Player(Bankroll bankroll)
      : bankroll(bankroll), currHand(0), dealerUpCard(std::nullopt) {}
  virtual void notifyShuffle() = 0;
  virtual void observeCard(const Card& card) = 0;
  virtual PlayerAction getNextAction() = 0;
  virtual Wager getWager() = 0;

  virtual void beginRound(const Card& firstCard, Wager wager);
  virtual void receiveCard(const Card& card);
  virtual void setDealerUpCard(const Card& card);

  virtual void splitCurrentHand();
  virtual void doubleCurrentHand();
  virtual void endCurrentHand();

  virtual void credit(Wager winnings);
  virtual void debit(Wager losses);

  Bankroll getBankroll();

  std::optional<ConstHandIter> getBeginHand();
  std::optional<ConstHandIter> getEndHand();

  bool playingLastHand();
  CardTotal getCurrentHandValue();
  Wager getCurrentHandWager();
  std::optional<ConstHandIter> getCurrentHand();

protected:
  Bankroll bankroll;
  HandIter currHand;
  std::optional<Card> dealerUpCard;
  std::list<Hand> hands;
};