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

bool Player::playingLastHand() {
  return currHand == hands.size() - 1;
}

CardTotal Player::getCurrentHandValue() {
  return hands[currHand].getValue();
}

Wager Player::getCurrentHandWager() {
  return hands[currHand].getWager();
}

void Player::endCurrentHand() {
  if (!playingLastHand()) {
    ++currHand;
  }
}

void Player::splitCurrentHand() {
  auto newHand = hands[currHand].split();
  hands.push_back(newHand);
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