#include <optional>
#include <vector>
#include "card.h"
#include "hand.h"

void Hand::reset() {
  isSplit = false;
  isDoubled = false;
  value = 0;
  wager = 0;

  cards.erase(cards.begin(), cards.end());
}
void Hand::addCard(Card card) {
  cards.push_back(card);
  value = sumCards(cards.cbegin(), cards.cend());
}

Hand Hand::split() {
  isSplit = true;
  cards.pop_back();
  value = sumCards(cards.cbegin(), cards.cend());

  auto newHand = Hand(*this);

  return newHand;
}

void Hand::doubleDown() {
  isDoubled = true;
  wager *= 2;
}

ConstCardIterator Hand::getBeginIter() const {
  return cards.cbegin();
}

ConstCardIterator Hand::getEndIter() const {
  return cards.cend();
}

Wager Hand::getWager() const {
  return wager;
}

Wager Hand::getValue() const {
  return value;
}

bool Hand::canDouble() const {
  return !isDoubled;
}

bool Hand::canSplit() const {
  return cards.size() == 2 && cards[0] == cards[1];
}

bool Hand::isBlackjack() const {
  return !isSplit && cards.size() == 2 && value == MAX_VALUE;
}

bool Hand::isSoft() const {
  // TODO: better way?
  // hand is soft if we can add 10 to it and the value doesn't change 
  std::vector<Card> tempCards { cards };
  tempCards.push_back(Card::Ten);

  auto newValue = sumCards(tempCards.begin(), tempCards.end());
  auto isSoft = newValue == value;

  return isSoft;
}

bool Hand::isBusted() const {
  return value > MAX_VALUE;
}