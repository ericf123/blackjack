#include <algorithm>
#include <array>
#include <chrono>
#include <optional>
#include <random>

#include "deck.h"

static constexpr const std::array<Card, 13> CARD_SET{
    {Card::Ace, Card::Two, Card::Three, Card::Four, Card::Five, Card::Six,
     Card::Seven, Card::Eight, Card::Nine, Card::Ten, Card::Jack, Card::Queen,
     Card::King}};

Deck::Deck(size_t numDecks) {
  const auto repeatTimes = numDecks * 4; // 4 of each card per deck
  for (auto curr = 0U; curr < repeatTimes; ++curr) {
    cards.insert(cards.end(), CARD_SET.cbegin(), CARD_SET.cend());
  }
  currCard = cards.cbegin();
}

size_t Deck::numCardsRemaining() {
  return std::distance(currCard, cards.cend());
}

std::optional<Card> Deck::draw() {
  if (currCard == cards.cend()) {
    return std::nullopt;
  } else {
    const auto card = *currCard;
    ++currCard;
    return {card};
  }
}

void Deck::shuffle() {
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
  currCard = cards.cbegin();
}