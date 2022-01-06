#include <stdlib.h>
#include <unistd.h>

#include "blackjack_events.h"
#include "hand.h"
#include "tui_player.h"
#include "view.h"

TuiPlayer::TuiPlayer(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
                     Bankroll bankroll)
    : Player(router, sourceNode, bankroll) {
  if (auto r = router.lock()) {
    EventHandler<void, TuiPlayerActionCmd> actionCmdHandler =
        [this](const WrappedEvent<void, TuiPlayerActionCmd>& e) {
          (void)e;
          const auto actionToSend = sanitizeAction(e.event.action);
          e.router.broadcast(this->sourceNode,
                             PlayerActionCmd<void>{ actionToSend });
        };

    EventHandler<Wager, PlayerGetWagerInv> wagerInvHandler =
        [this, r](const WrappedEvent<Wager, PlayerGetWagerInv>& e) -> Wager {
      (void)e;
      const auto wagerOpt = r->invokeFirstAvailable(
          this->sourceNode,
          // TODO: table minimum bet
          WagerViewGetWagerInv<Wager>{ 0, static_cast<Wager>(this->bankroll) });
      if (wagerOpt) {
        return wagerOpt.value();
      } else {
        return 0;
      }
    };

    EventHandler<void, PlayerEndHandCmd> endHandHandler =
        [this](const WrappedEvent<void, PlayerEndHandCmd>& e) {
          (void)e;
          if (!playingLastHand()) {
            if (currHand->isDoubled() || currHand->isBusted()) {
              e.router.broadcast(this->sourceNode,
                                 InputBlockUntilKeyPressed<void>{});
            }
            currHand = std::next(currHand);
          }
        };

    r->listen(sourceNode, false, wagerInvHandler);
    r->listen(sourceNode, false, actionCmdHandler);
    r->listen(sourceNode, false, endHandHandler);
  }
}

PlayerAction TuiPlayer::sanitizeAction(PlayerAction action) {
  switch (action) {
  case PlayerAction::Stand:
    if (playingLastHand()) {
      return PlayerAction::EndTurn;
    }

    return action;
  case PlayerAction::DoubleDown:
    if (!currHand->canDouble()) {
      return PlayerAction::InvalidInput;
    }

    return PlayerAction::DoubleDown;
  case PlayerAction::Split:
    if (!currHand->canSplit()) {
      return PlayerAction::InvalidInput;
    }

    return action;
  default:
    return action;
  }
}
