#pragma once

#include <memory>
#include "card.h"
#include "player.h"
#include "player_action.h"
#include "table_view.h"

class TuiPlayer : public Player {
public:
  TuiPlayer(double bankroll, std::shared_ptr<TableView> tableView) : Player(bankroll), tableView(tableView) {}
  virtual void observeCard(Card card) override;
  virtual PlayerAction getNextAction() override;
  virtual Wager getWager() override;
  virtual void receiveCard(Card card) override;
private:
  std::shared_ptr<TableView> tableView;
  PlayerAction getDesiredAction();
  PlayerAction sanitizeAction(PlayerAction action);
  void displayHands();
};