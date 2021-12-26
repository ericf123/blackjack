#pragma once

#include <memory>
#include "dealer.h"
#include "hand_view.h"
#include "table.h"
#include "tui.h"
#include "view.h"

class TableView : public View {
public:
  static constexpr size_t MAX_PLAYERS = 5;
  static constexpr size_t DEALER_HAND_INDEX = 0;
  TableView(std::shared_ptr<Table> table, std::shared_ptr<Dealer> dealer, 
            int starty, int startx);

  virtual void update() override;
private:
  std::unique_ptr<Hand> dummyHand;
  std::shared_ptr<Table> table;
  std::shared_ptr<Dealer> dealer;
  std::unique_ptr<std::array<HandView, MAX_PLAYERS + 1>> handViews;

  void drawIndividualHand(const Hand& hand, size_t index);
  void draw();
};