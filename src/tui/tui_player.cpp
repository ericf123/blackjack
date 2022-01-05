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
    EventHandler<PlayerActionReq> actionReqHandler =
        [this](const WrappedEvent<PlayerActionReq>& e) {
          (void)e;
          const auto actions = e.router.broadcastInvoke<PlayerAction>(
              this->sourceNode, InputPlayerActionInv{});
          auto actionToSend = PlayerAction::InvalidInput;
          if (!actions.empty()) {
            // there should only be one input node on a TUI
            actionToSend = sanitizeAction(actions.front());
          }

          // TODO: use specific dealer node instead of broadcast?
          e.router.broadcast(this->sourceNode,
                             PlayerActionResp{ actionToSend, *this });
        };

    InvokeHandler<Wager, PlayerGetWagerInv> wagerInvHandler =
        [this, r](const WrappedEvent<PlayerGetWagerInv>& e) -> Wager {
      (void)e;
      const auto wagerOpt = r->invokeFirstAvailable<Wager>(
          this->sourceNode,
          // TODO: table minimum bet
          WagerViewGetWagerInv{ 0, static_cast<Wager>(this->bankroll) });
      if (wagerOpt) {
        return wagerOpt.value();
      } else {
        return 0;
      }
    };

    r->registerInvokeHandler(sourceNode, actionReqHandler);
    r->registerInvokeHandler(sourceNode, wagerInvHandler);
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

// void TuiPlayer::splitCurrentHand() {
//   hands.push_back(currHand->split());
//   updateViews();
//   drawViewsToScreen();
// }

// void TuiPlayer::endCurrentHand() {
//   const auto justDoubled = currHand->isDoubled();
//   const auto finishedIntermediateSplitHand =
//       !playingLastHand() && currHand->isSplit();
//   if (justDoubled || finishedIntermediateSplitHand) {
//     getch();
//   }

//   if (!playingLastHand()) {
//     ++currHand;
//   }

//   updateViews();
//   drawViewsToScreen();
// }
