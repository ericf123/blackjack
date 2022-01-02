#include <stdlib.h>
#include <unistd.h>

#include "hand.h"
#include "tui_player.h"
#include "view.h"

void TuiPlayer::setCardsPerShoe(std::size_t cardsPerShoe) {
  hiLo.setCardsPerShoe(cardsPerShoe);
}

void TuiPlayer::notifyShuffle() {
  hiLo.reset();
  updateStatsViewCount();
  updateViews();
  drawViewsToScreen();
}

void TuiPlayer::observeCard(const Card& card) {
  hiLo.addCard(card);
  updateStatsViewCount();
  updateViews();
  drawViewsToScreen();
}

void TuiPlayer::receiveCard(const Card& card) {
  currHand->addCard(card);
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

PlayerAction TuiPlayer::getDesiredAction() {
  auto action = PlayerAction::InvalidInput;
  while (action == PlayerAction::InvalidInput) {
    auto userInputChar = getch();
    if (userInputChar == hitKey) {
      action = PlayerAction::Hit;
    } else if (userInputChar == standKey) {
      action = PlayerAction::Stand;
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
  hands.push_back(currHand->split());
  updateViews();
  drawViewsToScreen();
}

void TuiPlayer::endCurrentHand() {
  const auto justDoubled = currHand->isDoubled();
  const auto finishedIntermediateSplitHand =
      !playingLastHand() && currHand->isSplit();
  if (justDoubled || finishedIntermediateSplitHand) {
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

void TuiPlayer::attachStatsView(std::weak_ptr<StatsView> viewPtr) {
  statsView = viewPtr;
}

void TuiPlayer::updateViews() {
  if (!tableView.expired()) {
    tableView.lock()->update();
  }

  if (statsView && !statsView.value().expired()) {
    statsView.value().lock()->update();
  }

  // no need to update wager view as it's usually hidden
}

void TuiPlayer::updateStatsViewCount() {
  if (statsView && !statsView->expired()) {
    const auto sv = statsView.value().lock();
    sv->setRawCount(hiLo.getRawCount());
    sv->setTrueCount(hiLo.getTrueCount());
    sv->setDecksRemaining(hiLo.getDecksRemaining());
  }
}
