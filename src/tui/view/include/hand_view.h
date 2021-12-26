#pragma once

#include <array>
#include <optional>
#include "card.h"
#include "card_view.h"
#include "player.h"
#include "tui.h"
#include "view.h"

class HandView : public View {
public:
  static constexpr size_t MAX_CARDS = 6;

  HandView(const Hand* hand, int starty, int startx);

  void setHand(const Hand* hand);
  void setFirstCardVisible(bool visible);

  virtual void update() override;
  virtual void show() override;
  virtual void hide() override;
private:
  const Hand* hand;
  bool hideFirstCard;
  std::unique_ptr<std::array<CardView, MAX_CARDS>> cardViews;

  void renderHand();
};