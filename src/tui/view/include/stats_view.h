#pragma once

#include "player.h"
#include "tui.h"
#include "view.h"

class StatsView : public View {
public:
  StatsView(std::shared_ptr<Player> player, int starty, int startx);

  virtual void update() override;
  void setRawCount(int rawCount);
  void setTrueCount(int trueCount);
  void setDecksRemaining(double decksRemaining);

private:
  std::shared_ptr<Player> player;
  int rawCount;
  int trueCount;
  double decksRemaining;
  void draw();
};