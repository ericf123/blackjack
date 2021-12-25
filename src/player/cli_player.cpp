#include <cctype>
#include <iostream>
#include "cli_player.h"
#include "player_action.h"

void CliPlayer::observeCard(Card card) { return; }

PlayerAction CliPlayer::getNextAction() {
  displayHands();

  auto actualAction = PlayerAction::InvalidInput;

  while (actualAction == PlayerAction::InvalidInput) {
    actualAction = sanitizeAction(getDesiredAction());
  }

  return actualAction;
}

PlayerAction CliPlayer::getDesiredAction() {
  auto action = PlayerAction::InvalidInput;
  while (action == PlayerAction::InvalidInput) {
    char input;
    std::cout << "Enter action (H/S/D/Z): ";
    std::cin >> input;

    switch (toupper(input)) {
      case 'H':
        action = PlayerAction::Hit;
        break;
      case 'S':
        action = PlayerAction::Stay;
        break;
      case 'D':
        action = PlayerAction::DoubleDown;
        break;
      case 'Z':
        action = PlayerAction::Split;
        break;
      default:
        action = PlayerAction::InvalidInput;
        break;
    }
  }

  return action;
}


PlayerAction CliPlayer::sanitizeAction(PlayerAction action) {
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

void CliPlayer::displayHands() {
  std::cout << "-----Your Hands-----" << std::endl;
  for (auto i = 0; i < hands.size(); i++) {
    if (i == currHand) {
      std::cout << "* ";
    }

    std::cout << hands[i] << std::endl;
  }
  std::cout << "--------------------" << std::endl;
}

Wager CliPlayer::getWager() {
  return 100;
}
