#pragma once

#include <memory>
#include "card.h"
#include <ncurses.h>
#include "player.h"
#include "player_action.h"
#include "table_view.h"
#include "wager_view.h"

class TuiPlayer : public Player {
public:
  TuiPlayer(double bankroll, std::shared_ptr<TableView> tableView, std::shared_ptr<WagerView> wagerView, 
            chtype hitKey,
            chtype stayKey, chtype doubleKey, chtype splitKey) 
          : Player(bankroll), tableView(tableView), wagerView(wagerView), hitKey(hitKey), 
            stayKey(stayKey), doubleKey(doubleKey), splitKey(splitKey) {}

  TuiPlayer(double bankroll, std::shared_ptr<TableView> tableView, std::shared_ptr<WagerView> wagerView)
          : TuiPlayer(bankroll, tableView, wagerView, 'j', 'k', 'l', ';') {}

  virtual ~TuiPlayer() = default;

  virtual void observeCard(Card card) override;
  virtual PlayerAction getNextAction() override;
  virtual Wager getWager() override;

  virtual void receiveCard(Card card) override;
  virtual void splitCurrentHand() override;
  virtual void endCurrentHand() override;
private:
  std::shared_ptr<TableView> tableView;
  std::shared_ptr<WagerView> wagerView;
  PlayerAction getDesiredAction();
  PlayerAction sanitizeAction(PlayerAction action);

  int hitKey;
  int stayKey;
  int doubleKey;
  int splitKey;

  void displayHands();
};