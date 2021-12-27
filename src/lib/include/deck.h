#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "card.h"

using namespace bjcard;

class Deck {
public:
  Deck(size_t numDecks);
  size_t numCardsRemaining();
  std::optional<Card> draw();
  void shuffle();

private:
  static constexpr size_t DECK_SIZE = 52;
  std::vector<Card> cards;
  std::vector<Card>::const_iterator currCard;
};