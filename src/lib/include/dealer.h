#pragma once

#include "card.h"
#include "table.h"

using namespace bjcard;

class Dealer {
public:
  Dealer(std::shared_ptr<Table> table);
  void addPlayerToTable(std::shared_ptr<Player> player);

  std::optional<ConstHandIter> getDealerHand();

  void resetRound();
  void playRound();
  void dealInitialCards();
  bool checkDealerBlackjack();
  void runPlayerActions();
  void playDealerHand();
  void handleRoundResults();

private:
  const CardTotal DEALER_STAY_VALUE = 17;
  HandIter dealerHand;
  std::list<Hand> dealerHands;
  std::shared_ptr<Table> table;

  void publishUpCard(const Card& card);
  bool shouldPlayDealerHand();
  bool handlePlayerAction(Player& player, PlayerAction action);
};