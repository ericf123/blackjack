#include "hand.h"
#include "card.h"
#include <optional>
#include <vector>

void Hand::reset() {
  hasChild = false;
  isDoubledDown = false;
  value = 0;
  wager = 0;
  depth = 0;

  cards.erase(cards.begin(), cards.end());
}
void Hand::addCard(Card card) {
  cards.push_back(card);
  value = sumCards(cards.cbegin(), cards.cend());
}

Hand Hand::split() {
  hasChild = true;
  cards.pop_back();
  value = sumCards(cards.cbegin(), cards.cend());

  Hand newHand{ wager, depth + 1 };
  newHand.addCard(cards.front());

  return newHand;
}

void Hand::doubleDown() {
  isDoubledDown = true;
  wager *= 2;
}

ConstCardIterator Hand::getBeginIter() const { return cards.cbegin(); }

ConstCardIterator Hand::getEndIter() const { return cards.cend(); }

Wager Hand::getWager() const { return wager; }

Wager Hand::getValue() const { return value; }

int Hand::getDepth() const { return depth; }

bool Hand::canDouble() const { return !isDoubled() && cards.size() == 2; }

bool Hand::canSplit() const {
  // return cards.size() == 2 && cards[0] == cards[1];
  return true;
}

bool Hand::isSplit() const { return hasChild || depth > 0; }

bool Hand::isDoubled() const { return isDoubledDown; }

bool Hand::isBlackjack() const {
  return !isSplit() && cards.size() == 2 && value == MAX_VALUE;
}

bool Hand::isSoft() const {
  // TODO: better way?
  // hand is soft if we can add 10 to it and the value doesn't change
  std::vector<Card> tempCards{ cards };
  // tempCards.push_back(Card::Ten);

  auto newValue = sumCards(tempCards.begin(), tempCards.end());
  auto isSoft = newValue == value;

  return isSoft;
}

bool Hand::isBusted() const { return value > MAX_VALUE; }