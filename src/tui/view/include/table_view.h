#pragma once

#include "dealer.h"
#include "event_def.h"
#include "hand_view.h"
#include "table.h"
#include "tui.h"
#include "view.h"
#include <memory>

class TableView : public View {
public:
  static constexpr size_t MAX_PLAYERS = 5;
  static constexpr size_t DEALER_HAND_INDEX = 0;
  TableView(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
            int starty, int startx);

  virtual ~TableView() = default;

private:
  std::weak_ptr<EventRouter> router;
  OwningHandle sourceNode;
  std::unique_ptr<std::array<HandView, MAX_PLAYERS + 1>> handViews;

  void setDealerUpCardVisible(bool visibile);
  void draw();
  void drawIndividualHand(std::optional<ConstHandIter> beginHand,
                          std::optional<ConstHandIter> currHand,
                          std::optional<ConstHandIter> endHand, size_t index);
};