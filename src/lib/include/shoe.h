#pragma once

#include <cstdint>
#include <optional>
#include <random>
#include <vector>

#include "card.h"

using namespace bjcard;

class Shoe {
public:
  Shoe(size_t numDecks, size_t minCards);
  size_t numCardsRemaining();
  bool needsShuffle();
  Card draw();
  void shuffle();

private:
  size_t minCards;
  std::vector<Card> cards;
  std::vector<Card>::const_iterator currCard;
  uint64_t randSeed;
  std::ranlux48 randGenerator;
};