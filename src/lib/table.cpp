#include <optional>
#include "table.h"

Table::Table(size_t numDecks, double blackjackPayoutRatio, bool dealerHitSoft17) 
  : blackjackPayoutRatio(blackjackPayoutRatio), 
    dealerHitSoft17(dealerHitSoft17), firstRound(true), deck(numDecks) {}

void Table::addPlayer(std::unique_ptr<Player> player) {
  players.push_back(std::move(player));
}

PlayerPtrIter Table::getBeginPlayer() {
  return players.begin();
}

PlayerPtrIter Table::getEndPlayer() {
  return players.end();
}

void Table::showCardToPlayers(Card card) {
  for (auto& player : players) {
    player->observeCard(card);
  }
}

Card Table::drawCard(bool observable) {
  firstRound = false;
  auto card = deck.draw().value();

  if (observable) {
    showCardToPlayers(card);
  }

  return card;
}

Card Table::drawCard() {
  return drawCard(true);
}

void Table::shuffleIfNeeded() {
  const auto minCards = (players.size() + 1) * EST_MAX_PLAYER_CARDS_PER_ROUND;
  if (firstRound || deck.numCardsRemaining() < minCards) {
    forceShuffle();
  }
}

void Table::forceShuffle() {
  deck.shuffle();
}