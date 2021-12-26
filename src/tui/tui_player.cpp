#include <unistd.h>
#include <stdlib.h>

#include "tui_player.h"
#include "view.h"

void TuiPlayer::observeCard(Card card) {}

void TuiPlayer::receiveCard(Card card) {
  hands[currHand].addCard(card);
  tableView->update();
  drawViewsToScreen();
}

PlayerAction TuiPlayer::getNextAction() {
  auto actualAction = PlayerAction::InvalidInput;

  while (actualAction == PlayerAction::InvalidInput) {
    actualAction = sanitizeAction(getDesiredAction());
  }

  return actualAction;
}

PlayerAction TuiPlayer::getDesiredAction() {
  auto action = PlayerAction::InvalidInput;
  while (action == PlayerAction::InvalidInput) {
    auto userInputChar = getch();
    if (userInputChar == hitKey) {
      action = PlayerAction::Hit;
    } else if (userInputChar == stayKey) {
      action = PlayerAction::Stay;
    } else if (userInputChar == doubleKey) {
      action = PlayerAction::DoubleDown;
    } else if (userInputChar == splitKey) {
      action = PlayerAction::Split;
    } else {
      action = PlayerAction::InvalidInput;
    }
  }

  return action;
}

void TuiPlayer::endCurrentHand() {
  if (hands[currHand].isDoubled()) {
    getch();
  }

  if (!playingLastHand()) {
    ++currHand;
  }

  tableView->update();
  drawViewsToScreen();
}

PlayerAction TuiPlayer::sanitizeAction(PlayerAction action) {
  switch (action) {
    case PlayerAction::Stay:
      if (playingLastHand()) {
        return PlayerAction::EndTurn;
      }

      return action;
    case PlayerAction::DoubleDown:
      if (!hands[currHand].canDouble()) {
        return PlayerAction::InvalidInput;
      }

      return PlayerAction::DoubleDown;
    case PlayerAction::Split:
      if (!hands[currHand].canSplit()) {
        return PlayerAction::InvalidInput;
      }

      return action;
    default:
      return action;
  }
}

Wager TuiPlayer::getWager() {
  return wagerView->getWager();
}