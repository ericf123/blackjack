#include <cctype>
#include <limits>
#include <iomanip>
#include <iostream>
#include "cli_player.h"
#include "player_action.h"

void CliPlayer::observeCard(Card card) { /* cli player does nothing with observations */ }

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
  std::cout << "--------------------" << std::endl;
  std::cout << "Bankroll: $" << std::put_money(bankroll) << std::endl;
  std::cout << "-----Your Hands-----" << std::endl;
  for (auto i = 0; i < hands.size(); i++) {
    if (i == currHand) {
      std::cout << "* ";
    }

    std::cout << "($" << std::put_money(hands[i].getWager()) << ") ";
    std::cout << hands[i] << std::endl;
  }
  std::cout << "--------------------" << std::endl;
  std::cout << "Dealer Up: " << dealerUpCard.value() << std::endl;
  std::cout << "--------------------" << std::endl;
}

Wager CliPlayer::getWager() {
  Wager desiredWager = 0;
  while (std::cin.fail() || desiredWager == 0 || desiredWager > bankroll) {
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "--------------------" << std::endl;
    std::cout << "Bankroll: $" << std::put_money(bankroll) << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "Enter wager: ";
    std::cin >> desiredWager;
  }

  return static_cast<Wager>(desiredWager);
}
