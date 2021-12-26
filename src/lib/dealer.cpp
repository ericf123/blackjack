#include "dealer.h"

void Dealer::addPlayerToTable(std::shared_ptr<Player> player) {
  table->addPlayer(player);
}

void Dealer::resetRound() {
  dealerHand.reset();
}

void Dealer::playRound() {
  resetRound();
  dealInitialCards();

  const auto dealerHasBlackjack = checkDealerBlackjack();

  if (!dealerHasBlackjack) {
    runPlayerActions();
    playDealerHand();
  } 

  handleRoundResults();
}

const Hand& Dealer::getDealerHand() {
  return dealerHand;
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

  // deal first card to dealer (unobservable)
  dealerHand.addCard(table->drawCard(false));

  // deal everyone with a non-zero wager the second card
  for (auto playerIter = table->getBeginPlayer(); playerIter != table->getEndPlayer(); ++playerIter) {
    auto& player = **playerIter;
    if (player.getCurrentHandWager() > 0) {
      player.receiveCard(table->drawCard());
    }
  }

  // deal second card to dealer (observable)
  const auto upCard = table->drawCard();
  dealerHand.addCard(upCard);
  publishUpCard(upCard);
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

bool Dealer::shouldPlayDealerHand() {
  for (auto playerIter = table->getBeginPlayer(); playerIter != table->getEndPlayer(); ++playerIter) {
    auto& player = **playerIter;
    for(auto hand = player.getBeginHand(); hand != player.getEndHand(); ++hand) {
      if (!hand->isBusted() && !hand->isBlackjack()) {
        return true;
      }
    }
  }

  return false;
}

void Dealer::playDealerHand() {
  // allow players to observe the dealer's down card (they couldn't before)
  // this assumes the dealer hides their first card, shows their second
  const auto dealerDownCard = *dealerHand.getBeginIter();
  table->showCardToPlayers(dealerDownCard);

  if (shouldPlayDealerHand()) {
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

      if (hand->isBlackjack() && !dealerHand.isBlackjack()) {
        // pay blackjack payout + original wager
        const auto blackjackPayout = (1.0 + table->getBlackjackPayoutRatio()) * handWager;
        player.credit(blackjackPayout);
      } else if (handValue == dealerHandValue) {
        // push, just re-credit what we debitted earlier 
        player.credit(handWager);
      } else if ((dealerBusted && !playerBusted) || (!playerBusted && handValue > dealerHandValue)) {
        player.credit(2 * handWager);
      } // else -> player lost, do nothing
    }
  }
}
