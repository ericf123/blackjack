#include "stats_view.h"

StatsView::StatsView(std::shared_ptr<Player> player, int starty, int startx) :
  View(LINES - 2, bjdim::STATS_WIDTH, starty, startx), player(player) {
  wbkgd(window, COLOR_PAIR(bjcolor::PAIR_BKGD));
  box(window, 0, 0);
  draw();
}

void StatsView::draw() {
  mvwprintw(window, 1, 1, "Bankroll: $%d", static_cast<int>(player->getBankroll()));
}

void StatsView::update() {
  draw();
}
