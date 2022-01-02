#include "table.h"
#include <algorithm>
#include <optional>

Table::Table(size_t numDecks, size_t minCardsInShoe,
             double blackjackPayoutRatio, bool dealerHitSoft17)
    : blackjackPayoutRatio(blackjackPayoutRatio),
      dealerHitSoft17(dealerHitSoft17), firstRound(true),
      shoe(numDecks, minCardsInShoe) {}

void Table::addPlayer(std::shared_ptr<Player> player) {
  players.push_back(player);
}

PlayerPtrIter Table::getBeginPlayer() { return players.begin(); }

PlayerPtrIter Table::getEndPlayer() { return players.end(); }

void Table::showCardToPlayers(const Card& card) {
  for (auto& player : players) {
    player->observeCard(card);
  }
}

Card Table::drawCard(bool observable) {
  firstRound = false;
  const auto card = shoe.draw();

  if (observable) {
    showCardToPlayers(card);
  }

  return card;
}

Card Table::drawCard() { return drawCard(true); }

void Table::shuffleIfNeeded() {
  // TODO: shuffle with 2 decks left??
  if (firstRound || shoe.needsShuffle()) {
    forceShuffle();
    const auto notify = [](std::shared_ptr<Player> p) { p->notifyShuffle(); };
    std::for_each(players.begin(), players.end(), notify);
  }
}

void Table::forceShuffle() { shoe.shuffle(); }
