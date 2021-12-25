#include "dealer.h"

void Dealer::addPlayerToTable(std::unique_ptr<Player> player) {
  table->addPlayer(std::move(player));
}

void Dealer::playRound() {
  dealerHand.reset();
  dealInitialCards();

  const auto dealerHasBlackjack = checkDealerBlackjack();

  if (!dealerHasBlackjack) {
    runPlayerActions();
    playDealerHand();
  } else {
    std::cout << "DEALER BLACKJACK :(" << std::endl;
  }

  handleRoundResults();
}

void Dealer::dealInitialCards() {
  table->shuffleIfNeeded();

  // deal everyone with a non-zero wager the first card
  for (auto playerIter = table->getBeginPlayer(); playerIter != table->getEndPlayer(); ++playerIter) {
    auto& player = **playerIter;
    const auto wager = player.getWager();

    if (wager > 0) {
      player.debit(wager);
      player.beginRound(table->drawCard(), wager);
    }
  }

  // deal first card to dealer, show it to the table
  const auto firstDealerCard = table->drawCard();
  dealerHand.addCard(firstDealerCard);
  publishUpCard(firstDealerCard);

  // deal everyone with a non-zero wager the second card
  for (auto playerIter = table->getBeginPlayer(); playerIter != table->getEndPlayer(); ++playerIter) {
    auto& player = **playerIter;
    if (player.getCurrentHandWager() > 0) {
      player.receiveCard(table->drawCard());
    }
  }

  // deal second card to dealer (unobservable)
  dealerHand.addCard(table->drawCard(false));
}

void Dealer::publishUpCard(Card card) {
  for (auto playerIter = table->getBeginPlayer(); playerIter != table->getEndPlayer(); ++playerIter) {
    auto& player = **playerIter;
    player.setDealerUpCard(card);
  }
}

bool Dealer::checkDealerBlackjack() {
  return dealerHand.isBlackjack();
}

void Dealer::runPlayerActions() {
  for (auto playerIter = table->getBeginPlayer(); playerIter != table->getEndPlayer(); ++playerIter) {
    auto& player = **playerIter;
    auto action = player.getNextAction();
    while (handlePlayerAction(player, action)) {
      action = player.getNextAction();
    }
  }
}

bool Dealer::handlePlayerAction(Player& player, PlayerAction action) {
  auto drawCard = action == PlayerAction::Hit;

  if (action == PlayerAction::Split) {
      player.debit(player.getCurrentHandWager());
      player.splitCurrentHand();
      drawCard = true;
  }

  if (action == PlayerAction::DoubleDown) {
    player.debit(player.getCurrentHandWager());
    player.doubleCurrentHand();
    drawCard = true;
    if (player.playingLastHand()) {
      action = PlayerAction::EndTurn;
    } else {
      action = PlayerAction::Stay;
    }
  }

  if (drawCard) {
    player.receiveCard(table->drawCard());
  }

  // check if player busted
  if (player.getCurrentHandValue() > Hand::MAX_VALUE) {
    if (player.playingLastHand()) {
      action = PlayerAction::EndTurn;
    } else {
      action = PlayerAction::Stay;
    }
  }

  if (action == PlayerAction::Stay || action == PlayerAction::EndTurn) {
    player.endCurrentHand();
  }

  return action != PlayerAction::EndTurn;
}

bool Dealer::allPlayersBusted() {
  for (auto playerIter = table->getBeginPlayer(); playerIter != table->getEndPlayer(); ++playerIter) {
    auto& player = **playerIter;
    for(auto hand = player.getBeginHand(); hand != player.getEndHand(); ++hand) {
      if (!hand->isBusted()) {
        return false;
      }
    }
  }

  return true;
}

void Dealer::playDealerHand() {
  // allow players to observe the dealer's down card (they couldn't before)
  // this assumes the dealer shows their first card, hides their second
  const auto dealerDownCard = *(dealerHand.getBeginIter() + 1);
  table->showCardToPlayers(dealerDownCard);

  if (!allPlayersBusted()) {
    while (dealerHand.getValue() <= DEALER_STAY_VALUE) {
      if (dealerHand.getValue() < DEALER_STAY_VALUE) {
        dealerHand.addCard(table->drawCard());
      } else if (table->shouldDealerHitSoft17() && dealerHand.isSoft()) {
          dealerHand.addCard(table->drawCard());
      } else { // dealer has soft 17 and stays
        break;
      }
    }
  }

  std::cout << "final dealer hand: " << dealerHand << std::endl;
}

void Dealer::handleRoundResults() {
  for (auto playerIter = table->getBeginPlayer(); playerIter != table->getEndPlayer(); ++playerIter) {
    auto& player = **playerIter;
    for(auto hand = player.getBeginHand(); hand != player.getEndHand(); ++hand) {
      const auto handValue = hand->getValue();
      const auto handWager = hand->getWager();
      const auto dealerHandValue = dealerHand.getValue();
      const auto playerBusted = hand->isBusted();
      const auto dealerBusted = dealerHand.isBusted();

      if (handValue == dealerHandValue) {
        // push, just re-credit what we debitted earlier 
        // std::cout << "push" << std::endl;
        player.credit(handWager);
      } else if (hand->isBlackjack()) {
        // pay blackjack payout + original wager
        // std::cout << "blackjack!!" << std::endl;
        const auto blackjackPayout = (1.0 + table->getBlackjackPayoutRatio()) * handWager;
        player.credit(blackjackPayout);
      } else if ((dealerBusted && !playerBusted) || (!playerBusted && handValue > dealerHandValue)) {
        // std::cout << "winner!" << std::endl;
        player.credit(2 * handWager);
      } else {// else -> player lost, do nothing
        // std::cout << "loser :(" << std::endl;
      }
    }
  }
}
