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

  std::string displayStr{ "" };
  auto color_pair = COLOR_PAIR(bjcolor::PAIR_CARD_DOWN);

  if (card) {
    displayStr = getDisplayStr(card.value());
    color_pair = COLOR_PAIR(bjcolor::PAIR_CARD_BLACK);
  }

  wattron(window.get(), color_pair);
  wbkgd(window.get(), color_pair);
  box(window.get(), 0, 0);
  mvwprintw(window.get(), height / 2, width / 2, "%s", displayStr.c_str());
}

void CardView::update() { draw(); }

void CardView::setCard(std::optional<Card> newCard) { card = newCard; }
