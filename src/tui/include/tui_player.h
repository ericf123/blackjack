#pragma once

#include "card.h"
#include "player.h"
#include "player_action.h"
#include "stats_view.h"
#include "table_view.h"
#include "wager_view.h"
#include <memory>
#include <ncurses.h>

class TuiPlayer : public Player {
public:
  TuiPlayer(double bankroll, std::weak_ptr<TableView> tableView,
            std::weak_ptr<WagerView> wagerView,
            std::optional<std::weak_ptr<StatsView>> statsView, chtype hitKey,
            chtype standKey, chtype doubleKey, chtype splitKey)
      : Player(bankroll), tableView(tableView), wagerView(wagerView),
        statsView(statsView), hitKey(hitKey), standKey(standKey),
        doubleKey(doubleKey), splitKey(splitKey) {}

  TuiPlayer(double bankroll, std::weak_ptr<TableView> tableView,
            std::weak_ptr<WagerView> wagerView,
            std::optional<std::weak_ptr<StatsView>> statsView)
      : TuiPlayer(bankroll, tableView, wagerView, statsView, 'j', 'k', 'l',
                  ';') {}

  virtual ~TuiPlayer() = default;

  virtual void observeCard(const Card& card) override;
  virtual PlayerAction getNextAction() override;
  virtual Wager getWager() override;

  virtual void receiveCard(const Card& card) override;
  virtual void splitCurrentHand() override;
  virtual void endCurrentHand() override;

  void attachStatsView(std::weak_ptr<StatsView> statsView);

private:
  std::weak_ptr<TableView> tableView;
  std::weak_ptr<WagerView> wagerView;
  std::optional<std::weak_ptr<StatsView>> statsView;

  int hitKey;
  int standKey;
  int doubleKey;
  int splitKey;

  PlayerAction getDesiredAction();
  PlayerAction sanitizeAction(PlayerAction action);
  void updateViews();
};