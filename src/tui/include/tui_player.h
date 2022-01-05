#pragma once

#include "card.h"
#include "hi_lo.h"
#include "player.h"
#include "player_action.h"
#include "stats_view.h"
#include "table_view.h"
#include "wager_view.h"
#include <memory>
#include <ncurses.h>

class TuiPlayer : public Player {
public:
  TuiPlayer(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
            Bankroll bankroll);

  // TODO: necessary?
  virtual ~TuiPlayer() = default;

private:
  PlayerAction sanitizeAction(PlayerAction action);
};