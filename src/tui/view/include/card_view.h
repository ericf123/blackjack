#pragma once

#include "card.h"
#include "tui.h"
#include "view.h"
#include <ncurses.h>
#include <optional>

class CardView : public View {
public:
  CardView(std::optional<Card> card, int starty, int startx);
  void setCard(std::optional<Card> card);
  virtual void update() override;

private:
  void draw();
  std::optional<Card> card;
};