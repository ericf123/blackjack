#pragma once

#include "card.h"
#include "player.h"
#include "player_action.h"
#include "table_view.h"
#include "wager_view.h"
#include <memory>
#include <ncurses.h>

class TuiPlayer : public Player {
public:
  TuiPlayer(double bankroll, std::weak_ptr<TableView> tableView,
            std::weak_ptr<WagerView> wagerView, chtype hitKey, chtype stayKey,
            chtype doubleKey, chtype splitKey)
      : Player(bankroll), tableView(tableView), wagerView(wagerView),
        hitKey(hitKey), stayKey(stayKey), doubleKey(doubleKey),
        splitKey(splitKey) {}

  TuiPlayer(double bankroll, std::shared_ptr<TableView> tableView,
            std::shared_ptr<WagerView> wagerView)
      : TuiPlayer(bankroll, tableView, wagerView, 'j', 'k', 'l', ';') {}

  virtual ~TuiPlayer() = default;

  virtual void observeCard(const Card& card) override;
  virtual PlayerAction getNextAction() override;
  virtual Wager getWager() override;

  virtual void receiveCard(const Card& card) override;
  virtual void splitCurrentHand() override;
  virtual void endCurrentHand() override;

private:
  std::weak_ptr<TableView> tableView;
  std::weak_ptr<WagerView> wagerView;

  int hitKey;
  int stayKey;
  int doubleKey;
  int splitKey;

  PlayerAction getDesiredAction();
  PlayerAction sanitizeAction(PlayerAction action);
  void updateViews();
};