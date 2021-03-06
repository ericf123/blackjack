#pragma once

#include "card.h"
#include "tui.h"
#include "view.h"
#include <ncurses.h>
#include <optional>

using namespace bjcard;

class CardView : public View {
public:
  CardView(std::optional<Card> card, int starty, int startx);
  void setCard(std::optional<Card> card);
  void update();

private:
  void draw();
  std::optional<Card> card;
};