#pragma once

#include "dealer.h"
#include "hand_view.h"
#include "table.h"
#include "tui.h"
#include "view.h"
#include <memory>

class TableView : public View {
public:
  static constexpr size_t MAX_PLAYERS = 5;
  static constexpr size_t DEALER_HAND_INDEX = 0;
  TableView(std::shared_ptr<Table> table, std::shared_ptr<Dealer> dealer,
            int starty, int startx);

  virtual void update() override;

  void setDealerUpCardVisible(bool visibility);

private:
  std::shared_ptr<Table> table;
  std::shared_ptr<Dealer> dealer;
  std::unique_ptr<std::array<HandView, MAX_PLAYERS + 1>> handViews;

  void draw();
  void drawIndividualHand(const Hand &hand, size_t index);
};