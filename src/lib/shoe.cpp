#include "shoe.h"
#include "card.h"
#include <algorithm>
#include <chrono>
#include <random>

Shoe::Shoe(size_t numDecks, size_t minCards) : minCards(minCards) {
  const auto repeatTimes = numDecks;
  for (auto curr = 0U; curr < repeatTimes; ++curr) {
    for (auto rank = Rank::Ace; rank <= Rank::King; ++rank) {
      for (auto suit = Suit::Hearts; suit <= Suit::Clubs; ++suit) {
        cards.push_back(Card{ rank, suit });
      }
    }
  }
  currCard = cards.cbegin();

  // use both random device and time to seed generator in case
  // `std::random_device` is a PRNG
  std::random_device rd;
  const auto now =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  randSeed = now ^ rd();
  randGenerator = std::ranlux48{ randSeed };
}

size_t Shoe::numCardsRemaining() {
  return std::distance(currCard, cards.cend());
}

bool Shoe::needsShuffle() { return numCardsRemaining() <= minCards; }

Card Shoe::draw() {
  // this is only as a worst case scenario
  // in general, the shoe should only be reshuffled between hands, using
  // `needsShuffle` to determine if a shuffle is required
  if (currCard == cards.cend()) {
    shuffle();
  }

  const auto card = *currCard;
  ++currCard;
  return { card };
}

void Shoe::shuffle() {
  std::shuffle(cards.begin(), cards.end(), randGenerator);
  currCard = cards.cbegin();
}