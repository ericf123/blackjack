#include "player.h"

void Player::receiveCard(Card card) {
  hands[currHand].addCard(card);
}

void Player::setDealerUpCard(Card card) {
  dealerUpCard.emplace(card);
}

void Player::beginRound(Card firstCard, Wager wager) {
  hands.erase(hands.begin(), hands.end());

  hands.push_back(Hand(wager));
  currHand = 0;

  receiveCard(firstCard);
}

void Player::credit(Wager winnings) {
  bankroll += winnings;
}

void Player::debit(Wager losses) {
  bankroll -= losses;
}

Bankroll Player::getBankroll() {
  return bankroll;
}

bool Player::playingLastHand() {
  return currHand == hands.size() - 1;
}

CardTotal Player::getCurrentHandValue() {
  if (currHand < hands.size()) {
    return hands[currHand].getValue();
  }

  return 0;
}

Wager Player::getCurrentHandWager() {
  if (currHand < hands.size()) {
    return hands[currHand].getWager();
  }

  return 0;
}

const Hand* Player::getCurrentHand() {
  if (currHand < hands.size()) {
    return &hands[currHand];
  }

  return nullptr;
}

void Player::endCurrentHand() {
  if (!playingLastHand()) {
    ++currHand;
  }
}

void Player::splitCurrentHand() {
  hands.push_back(hands[currHand].split());
}

void Player::doubleCurrentHand() {
  hands[currHand].doubleDown();
}

ConstHandIter Player::getBeginHand() {
  return hands.cbegin();
}

ConstHandIter Player::getEndHand() {
  return hands.cend();
}