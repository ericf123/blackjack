#include "stats_view.h"
#include "blackjack_events.h"
#include "event_router.h"

StatsView::StatsView(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
                     int starty, int startx)
    : View(LINES - 2, bjdim::STATS_WIDTH, starty, startx), router(router),
      sourceNode(sourceNode), rawCount(0), trueCount(0), decksRemaining(0) {
  if (auto r = router.lock()) {
    EventHandler<ViewUpdateCmd> updateHandler =
        [this](const WrappedEvent<ViewUpdateCmd>& e) {
          (void)e;
          draw();
        };

    r->listen(sourceNode, false, updateHandler);
  }

  wbkgd(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD));
  box(window.get(), 0, 0);
  draw();
}

void StatsView::draw() {
  if (auto r = router.lock()) {
    const auto playerOpt =
        r->invokeFirstAvailable<std::reference_wrapper<const Player>>(
            sourceNode, ToConstRefInv<Player>{});

    if (playerOpt) {
      const auto& player = playerOpt.value().get();
      mvwprintw(window.get(), 1, 1, "Bankroll: $       "); // TODO: fix this
      mvwprintw(window.get(), 1, 1, "Bankroll: $%0.0f", player.getBankroll());
    }
    // mvwprintw(window.get(), 2, 1,
    //           "Raw Count:     "); // TODO: fix this (4 spaces)
    // mvwprintw(window.get(), 2, 1, "Raw Count: %+d", rawCount);
    // mvwprintw(window.get(), 3, 1,
    //           "True Count:     "); // TODO: fix this (4 spaces)
    // mvwprintw(window.get(), 3, 1, "True Count: %+d", trueCount);
    // mvwprintw(window.get(), 4, 1,
    //           "Decks Remaining:     "); // TODO: fix this (4 spaces)
    // mvwprintw(window.get(), 4, 1, "Decks Remaining: %0.1f",
    // decksRemaining);
  }
}

void StatsView::setRawCount(int rawCount) { this->rawCount = rawCount; }
void StatsView::setTrueCount(int trueCount) { this->trueCount = trueCount; }
void StatsView::setDecksRemaining(double decksRemaining) {
  this->decksRemaining = decksRemaining;
}
