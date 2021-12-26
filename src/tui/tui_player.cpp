#include <unistd.h>
#include <stdlib.h>

#include "tui_player.h"
#include "view.h"

void TuiPlayer::observeCard(Card card) {

}

void TuiPlayer::receiveCard(Card card) {
  hands[currHand].addCard(card);
  tableView->update();
  drawViewsToScreen();
  getch();
}

PlayerAction TuiPlayer::getNextAction() {
  drawViewsToScreen();
  return PlayerAction::EndTurn;
}

Wager TuiPlayer::getWager() {
  return rand() % 100;
}