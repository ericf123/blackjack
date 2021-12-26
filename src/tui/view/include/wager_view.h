#pragma once

#include <form.h>
#include "hand.h"
#include "tui.h"
#include "view.h"

class WagerView : public View {
public:
  WagerView(int starty, int startx);
  WagerView(WagerView&& view);
  virtual ~WagerView() override;
  virtual void update() override {}
  Wager getWager();
private:
  static constexpr size_t NUM_FIELDS = 1; 
  Wager prevWager;
  // TODO: use unique_ptr
  FORM* form;
  std::array<FIELD*, NUM_FIELDS + 1> fields;
};