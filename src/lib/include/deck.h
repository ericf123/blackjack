#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "card.h"

class Deck {
public:
  Deck(size_t numDecks);
  size_t numCardsRemaining();
  std::optional<Card> draw();
  void shuffle();

private:
  std::vector<Card> cards;
  std::vector<Card>::const_iterator currCard;
};