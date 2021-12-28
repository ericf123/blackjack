#pragma once

#include "card.h"
#include "card_view.h"
#include "player.h"
#include "tui.h"
#include "view.h"
#include <array>
#include <optional>

class HandView : public View {
public:
  static constexpr size_t MAX_CARDS = 8;

  HandView(std::optional<ConstHandIter> hand, int starty, int startx);

  void setHandRange(std::optional<ConstHandIter> beginHand,
                    std::optional<ConstHandIter> hand,
                    std::optional<ConstHandIter> endHand);
  void setFirstCardVisible(bool visible);

  virtual void update() override;
  virtual void show() override;
  virtual void hide() override;

private:
  std::optional<ConstHandIter> beginHand;
  std::optional<ConstHandIter> endHand;
  std::optional<ConstHandIter> hand;
  bool hideFirstCard;
  std::unique_ptr<std::array<CardView, MAX_CARDS>> cardViews;

  void renderHand();
};