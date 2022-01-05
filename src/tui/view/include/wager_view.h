#pragma once

#include "event_def.h"
#include "form_view.h"
#include "hand.h"
#include "tui.h"
#include "view.h"
#include <form.h>

class WagerView : public FormView<1> {
public:
  WagerView(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
            int starty, int startx);
  virtual ~WagerView() = default;

  Wager getWager();
  // void setMinWager(Wager wager);
  // void setMaxWager(Wager wager);

private:
  std::weak_ptr<EventRouter> router;
  OwningHandle sourceNode;
  Wager prevWager;
  Wager minWager;
  Wager maxWager;

  void setWagerRange(Wager min, Wager max);
};