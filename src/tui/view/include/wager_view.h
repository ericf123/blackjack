#pragma once

#include "hand.h"
#include "tui.h"
#include "view.h"
#include <form.h>

class WagerView : public View {
public:
  WagerView(int starty, int startx);
  WagerView(WagerView &&view);
  virtual ~WagerView() override;
  virtual void update() override {}
  Wager getWager();
  void setMinWager(Wager wager);
  void setMaxWager(Wager wager);

private:
  static constexpr size_t NUM_FIELDS = 1;
  Wager prevWager;
  Wager minWager;
  Wager maxWager;
  // TODO: use unique_ptr
  FORM *form;
  std::array<FIELD *, NUM_FIELDS + 1> fields;

  void setWagerRange(Wager min, Wager max);
};