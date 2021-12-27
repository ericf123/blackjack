#include "card_view.h"
#include "card.h"
#include "tui.h"
#include <cstdint>
#include <ncurses.h>
#include <optional>
#include <string>

CardView::CardView(std::optional<Card> card, int starty, int startx)
    : View(bjdim::CARD_HEIGHT, bjdim::CARD_WIDTH, starty, startx), card(card) {
  draw();
}

void CardView::draw() {
  // TODO: less hacky way
  wclear(window.get()); // otherwise 10 leaves its 0 when you go back to one
                        // char card

  auto color_pair = COLOR_PAIR(bjcolor::PAIR_CARD_DOWN);

  auto suit = Suit::Hearts;
  if (card) {
    suit = std::visit([](const auto& c) -> Suit { return c.suit; }, card.value());
    if (suit == Suit::Hearts || suit == Suit::Diamonds) {
      color_pair = COLOR_PAIR(bjcolor::PAIR_CARD_RED);
    } else {
      color_pair = COLOR_PAIR(bjcolor::PAIR_CARD_BLACK);
    }
  }

  wattron(window.get(), color_pair);
  wbkgd(window.get(), color_pair);
  box(window.get(), 0, 0);

  if (card) {
    std::ostringstream rankDisplay;
    rankDisplay << std::visit([](const auto& c) -> Rank { return c.rank; }, card.value());

    std::ostringstream suitDisplay;
    suitDisplay << suit;

    wattron(window.get(), A_BOLD);
    mvwprintw(window.get(), 1, 1, "%s", suitDisplay.str().c_str());
    mvwprintw(window.get(), height - 2, width - 2, "%s", suitDisplay.str().c_str());
    mvwprintw(window.get(), height / 2, width / 2, "%s", rankDisplay.str().c_str());
    wattroff(window.get(), A_BOLD);
  }
}

void CardView::update() { draw(); }

void CardView::setCard(std::optional<Card> newCard) { card = newCard; }
