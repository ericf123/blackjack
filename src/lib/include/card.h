#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>
#include <variant>
#include <sstream>

namespace bjcard {

using CardTotal = uint32_t;

enum class Rank {
  Ace = 1,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Ten,
  Jack,
  Queen,
  King
};

enum class Suit { Hearts, Diamonds, Spades, Clubs };

constexpr Rank indexToRank(size_t i) { return static_cast<Rank>(i); }

constexpr Suit indexToSuit(size_t i) { return static_cast<Suit>(i); }

template <Rank RANK, Suit SUIT> struct _Card {
  static constexpr Rank rank = RANK;
  static constexpr Suit suit = SUIT;
};

using Card = std::variant<_Card<Rank::Ace,   Suit::Hearts>,
                          _Card<Rank::Two,   Suit::Hearts>,
                          _Card<Rank::Three, Suit::Hearts>,
                          _Card<Rank::Four,  Suit::Hearts>,
                          _Card<Rank::Five,  Suit::Hearts>,
                          _Card<Rank::Six,   Suit::Hearts>,
                          _Card<Rank::Seven, Suit::Hearts>,
                          _Card<Rank::Eight, Suit::Hearts>,
                          _Card<Rank::Nine,  Suit::Hearts>,
                          _Card<Rank::Ten,   Suit::Hearts>,
                          _Card<Rank::Jack,  Suit::Hearts>,
                          _Card<Rank::Queen, Suit::Hearts>,
                          _Card<Rank::King,  Suit::Hearts>,
                          _Card<Rank::Ace,   Suit::Diamonds>,
                          _Card<Rank::Two,   Suit::Diamonds>,
                          _Card<Rank::Three, Suit::Diamonds>,
                          _Card<Rank::Four,  Suit::Diamonds>,
                          _Card<Rank::Five,  Suit::Diamonds>,
                          _Card<Rank::Six,   Suit::Diamonds>,
                          _Card<Rank::Seven, Suit::Diamonds>,
                          _Card<Rank::Eight, Suit::Diamonds>,
                          _Card<Rank::Nine,  Suit::Diamonds>,
                          _Card<Rank::Ten,   Suit::Diamonds>,
                          _Card<Rank::Jack,  Suit::Diamonds>,
                          _Card<Rank::Queen, Suit::Diamonds>,
                          _Card<Rank::King,  Suit::Diamonds>,
                          _Card<Rank::Ace,   Suit::Spades>,
                          _Card<Rank::Two,   Suit::Spades>,
                          _Card<Rank::Three, Suit::Spades>,
                          _Card<Rank::Four,  Suit::Spades>,
                          _Card<Rank::Five,  Suit::Spades>,
                          _Card<Rank::Six,   Suit::Spades>,
                          _Card<Rank::Seven, Suit::Spades>,
                          _Card<Rank::Eight, Suit::Spades>,
                          _Card<Rank::Nine,  Suit::Spades>,
                          _Card<Rank::Ten,   Suit::Spades>,
                          _Card<Rank::Jack,  Suit::Spades>,
                          _Card<Rank::Queen, Suit::Spades>,
                          _Card<Rank::King,  Suit::Spades>,
                          _Card<Rank::Ace,   Suit::Clubs>,
                          _Card<Rank::Two,   Suit::Clubs>,
                          _Card<Rank::Three, Suit::Clubs>,
                          _Card<Rank::Four,  Suit::Clubs>,
                          _Card<Rank::Five,  Suit::Clubs>,
                          _Card<Rank::Six,   Suit::Clubs>,
                          _Card<Rank::Seven, Suit::Clubs>,
                          _Card<Rank::Eight, Suit::Clubs>,
                          _Card<Rank::Nine,  Suit::Clubs>,
                          _Card<Rank::Ten,   Suit::Clubs>,
                          _Card<Rank::Jack,  Suit::Clubs>,
                          _Card<Rank::Queen, Suit::Clubs>,
                          _Card<Rank::King,  Suit::Clubs>>;


template<size_t N> struct CardSet {
  CardSet<N - 1> remaining;
  static constexpr Card card = std::variant_alternative_t<N - 1, Card>();
  constexpr Card operator[](const size_t i) const { return i == N - 1 ? card : remaining[i]; }
};

template<>
struct CardSet<1> {
  static constexpr Card card = std::variant_alternative_t<0, Card>();
  constexpr Card operator[](const size_t i) const { (void) i; return card; }
};

static const CardSet<std::variant_size_v<Card>> CARD_SET;

constexpr Card indexToCard(size_t i) {
  return CARD_SET[i];
}

static const std::string RANK_DISPLAY_STRS[] = { "?",  "A", "2", "3", "4",
                                                 "5",  "6", "7", "8", "9",
                                                 "10", "J", "Q", "K" };

// hearts, diamonds, spades, clubs
static const std::string SUIT_DISPLAY_STRS[] = { "\u2661", "\u2662", "\u2660", "\u2663" };

static inline std::ostream& operator<<(std::ostream& os, Rank rank) {
  os << RANK_DISPLAY_STRS[static_cast<size_t>(rank)];
  return os;
}

static inline std::ostream& operator<<(std::ostream& os, Suit suit) {
  os << SUIT_DISPLAY_STRS[static_cast<size_t>(suit)];
  return os;
}

static inline std::string getDisplayStr(Card card) {
  std::ostringstream oss;
  std::visit([&oss](const auto& arg) { oss << arg.rank << arg.suit; }, card);
  return oss.str();
}

static inline std::ostream& operator<<(std::ostream& os, Card card) {
  const auto strToWrite = getDisplayStr(card);
  os.write(strToWrite.c_str(), strToWrite.size());
  return os;
}

template <typename ItType> CardTotal sumCards(ItType begin, ItType end) {
  (void) begin;
  (void) end;
  return 0;
  // auto total = 0U;
  // auto numAces = 0;

  // for (auto curr = begin; curr != end; ++curr) {
  //   auto cardEnumValue = static_cast<CardTotal>(*curr);
  //   total += std::min(cardEnumValue, 10U);

  //   if (*curr == Card::Ace) {
  //     ++numAces;
  //   }
  // }

  // // can have at most one Ace with value of 11
  // // by default Ace has value of 1
  // if (numAces > 0 && total + 10 <= 21) {
  //   total += 10;
  // }

  // return total;
}

} // namespace bjcard