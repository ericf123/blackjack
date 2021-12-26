#pragma once

#include "card.h"
#include "table.h"

class Dealer {
public:
  Dealer(std::shared_ptr<Table> table)
      : firstRound(true), table(std::move(table)) {}
  void addPlayerToTable(std::shared_ptr<Player> player);

  const Hand &getDealerHand();

  void resetRound();
  void playRound();
  void getWagers();
  void dealInitialCards();
  bool checkDealerBlackjack();
  void runPlayerActions();
  void playDealerHand();
  void handleRoundResults();

private:
  const CardTotal DEALER_STAY_VALUE = 17;
  bool firstRound;
  Hand dealerHand;
  std::shared_ptr<Table> table;

  void publishUpCard(Card card);
  bool shouldPlayDealerHand();
  bool handlePlayerAction(Player &player, PlayerAction action);
};