#include <stdlib.h>
#include <unistd.h>

#include "tui_player.h"
#include "view.h"

void TuiPlayer::observeCard(Card card) { (void)card; }

void TuiPlayer::receiveCard(Card card) {
  hands[currHand].addCard(card);
  updateViews();
  drawViewsToScreen();
}

PlayerAction TuiPlayer::getNextAction() {
  auto actualAction = PlayerAction::InvalidInput;

  while (actualAction == PlayerAction::InvalidInput) {
    actualAction = sanitizeAction(getDesiredAction());
  }

  return actualAction;
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

void TuiPlayer::splitCurrentHand() {
  hands.push_back(hands[currHand].split());
  updateViews();
  drawViewsToScreen();
}

void TuiPlayer::endCurrentHand() {
  if (currHand < hands.size() && hands[currHand].isDoubled()) {
    getch();
  }

  if (!playingLastHand()) {
    ++currHand;
  }

  updateViews();
  drawViewsToScreen();
}

Wager TuiPlayer::getWager() {
  auto wager = 0;

  if (!wagerView.expired()) {
    auto ownedWagerView = wagerView.lock();
    ownedWagerView->setMaxWager(static_cast<Wager>(bankroll));
    wager = ownedWagerView->getWager();
  }

  return wager;
}

void TuiPlayer::updateViews() {
  if (!tableView.expired()) {
    tableView.lock()->update();
  }
}
