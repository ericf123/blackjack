#pragma once

#include "card.h"
#include "table.h"

class Dealer {
public:
  Dealer(std::shared_ptr<Table> table) : firstRound(true), table(std::move(table)) {}
  void addPlayerToTable(std::shared_ptr<Player> player);
  const Hand& getDealerHand();
  void playRound();
private:
  const CardTotal DEALER_STAY_VALUE = 17;
  bool firstRound;
  Hand dealerHand;
  std::shared_ptr<Table> table;

  void getWagers();
  void dealInitialCards();
  void publishUpCard(Card card);
  bool allPlayersBusted();
  bool checkDealerBlackjack();
  void runPlayerActions();
  bool handlePlayerAction(Player& player, PlayerAction action);
  void playDealerHand();
  void handleRoundResults();
};