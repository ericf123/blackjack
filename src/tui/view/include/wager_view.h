#pragma once

#include "form_view.h"
#include "hand.h"
#include "tui.h"
#include "view.h"
#include <form.h>

class WagerView : public FormView<1> {
public:
  WagerView(int starty, int startx);
  virtual ~WagerView() = default;
  virtual void update() override {}

  Wager getWager();
  void setMinWager(Wager wager);
  void setMaxWager(Wager wager);

private:
  Wager prevWager;
  Wager minWager;
  Wager maxWager;

  void setWagerRange(Wager min, Wager max);
};