#include "player.h"
#include <optional>

void Player::receiveCard(const Card& card) { currHand->addCard(card); }

void Player::setDealerUpCard(const Card& card) { dealerUpCard.emplace(card); }

void Player::beginRound(const Card& firstCard, Wager wager) {
  hands.clear();

  hands.push_front(Hand(wager));
  currHand = hands.begin();

  receiveCard(firstCard);
}

void Player::credit(Wager winnings) { bankroll += winnings; }

void Player::debit(Wager losses) { bankroll -= losses; }

Bankroll Player::getBankroll() { return bankroll; }

bool Player::playingLastHand() { return std::next(currHand) == hands.end(); }

CardTotal Player::getCurrentHandValue() {
  if (currHand != hands.end()) {
    return currHand->getValue();
  }

  return 0;
}

Wager Player::getCurrentHandWager() {
  if (currHand != hands.end()) {
    return currHand->getWager();
  }

  return 0;
}

std::optional<ConstHandIter> Player::getCurrentHand() {
  if (!hands.empty()) {
    return currHand;
  }

  return std::nullopt;
}

void Player::endCurrentHand() {
  if (!playingLastHand()) {
    ++currHand;
  }
}

void Player::splitCurrentHand() { hands.push_back(currHand->split()); }

void Player::doubleCurrentHand() { return currHand->doubleDown(); }

std::optional<ConstHandIter> Player::getBeginHand() {
  if (!hands.empty()) {
    return hands.cbegin();
  }

  return std::nullopt;
}

std::optional<ConstHandIter> Player::getEndHand() {
  if (!hands.empty()) {
    return hands.cend();
  }

  return std::nullopt;
}