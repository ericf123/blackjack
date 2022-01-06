#pragma once

#include "player.h"
#include "tui.h"
#include "view.h"

class StatsView : public View {
public:
  StatsView(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
            int starty, int startx);

private:
  std::weak_ptr<EventRouter> router;
  OwningHandle sourceNode;
  int rawCount;
  int trueCount;
  double decksRemaining;

  void draw();
  void setRawCount(int rawCount);
  void setTrueCount(int trueCount);
  void setDecksRemaining(double decksRemaining);
};