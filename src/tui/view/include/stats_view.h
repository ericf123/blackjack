#include "player.h"
#include "tui.h"
#include "view.h"

class StatsView : public View {
public:
  StatsView(std::shared_ptr<Player> player, int starty, int startx);

  virtual void update() override;
private:
  std::shared_ptr<Player> player;
  void draw();
};