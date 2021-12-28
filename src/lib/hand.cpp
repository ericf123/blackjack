#include "hand.h"
#include "card.h"
#include <optional>
#include <vector>

void Hand::reset() {
  isDoubledDown = false;
  value = 0;
  wager = 0;

  cards.erase(cards.begin(), cards.end());
}

void Hand::addCard(const Card& card) {
  cards.push_back(card);
  value = sumCards(cards.cbegin(), cards.cend());
}

Hand Hand::split() {
  isHandSplit = true;

  cards.pop_back();
  value = sumCards(cards.cbegin(), cards.cend());

  Hand newHand{ wager };
  newHand.isHandSplit = true;
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

bool Hand::canDouble() const { return !isDoubled() && cards.size() == 2; }

bool Hand::canSplit() const {
  // only cards 10 or higher can have different ranks and still be splitable
  // e.g. split J, Q
  const auto clippedFirstRank = std::min(cards[0].rank, Rank::Ten);
  const auto clippedSecondRank = std::min(cards[1].rank, Rank::Ten);
  return cards.size() == 2 && clippedFirstRank == clippedSecondRank;
}

bool Hand::isSplit() const { return isHandSplit; }

bool Hand::isDoubled() const { return isDoubledDown; }

bool Hand::isBlackjack() const {
  return !isSplit() && cards.size() == 2 && value == MAX_VALUE;
}

bool Hand::isSoft() const {
  // TODO: better way?
  // hand is soft if we can add 10 to it and the value doesn't change
  std::vector<Card> tempCards{ cards };
  tempCards.push_back(Card{ Rank::Ten, Suit::Spades });

  auto newValue = sumCards(tempCards.begin(), tempCards.end());
  auto isSoft = newValue == value;

  return isSoft;
}

bool Hand::isBusted() const { return value > MAX_VALUE; }