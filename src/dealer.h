#pragma once

#include "card.h"
#include "table.h"

class Dealer {
public:
  Dealer(std::unique_ptr<Table> table) : firstRound(true), table(std::move(table)) {}
  void addPlayerToTable(std::unique_ptr<Player> player);
  void playRound();
private:
  const CardTotal DEALER_STAY_VALUE = 17;
  bool firstRound;
  Hand dealerHand;
  std::unique_ptr<Table> table;

  void getWagers();
  void dealInitialCards();
  bool checkDealerBlackjack();
  void runPlayerActions();
  bool handlePlayerAction(Player& player, PlayerAction action);
  void playDealerHand();
  void handleRoundResults();
};