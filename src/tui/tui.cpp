#include "tui.h"
#include "blackjack_events.h"
#include "card.h"
#include "card_view.h"
#include "dealer.h"
#include "hand.h"
#include "hand_view.h"
#include "input_node.h"
#include "stats_view.h"
#include "table.h"
#include "table_view.h"
#include "title_view.h"
#include "tui_player.h"
#include "view.h"
#include "wager_view.h"
#include <locale.h>
#include <memory>
#include <ncurses.h>
#include <panel.h>

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  curs_set(0); // hide cursor

  if (has_colors()) {
    start_color();
    init_color(bjcolor::BKGD_GREEN, 0, 153, 0);
    init_color(bjcolor::CARD_RED, 625, 0, 0);
    init_pair(bjcolor::PAIR_BKGD, COLOR_WHITE, bjcolor::BKGD_GREEN);
    init_pair(bjcolor::PAIR_BKGD_INV, bjcolor::BKGD_GREEN, COLOR_WHITE);
    init_pair(bjcolor::PAIR_CARD_BLACK, COLOR_BLACK, COLOR_WHITE);
    init_pair(bjcolor::PAIR_CARD_RED, bjcolor::CARD_RED, COLOR_WHITE);
    init_pair(bjcolor::PAIR_CARD_DOWN, COLOR_WHITE, bjcolor::CARD_RED);
  }
  // set up main window background
  bkgd(COLOR_PAIR(bjcolor::PAIR_BKGD));
  attron(COLOR_PAIR(bjcolor::PAIR_BKGD));
  box(stdscr, 0, 0);
  refresh();

  // create models
  const auto DECKS_PER_SHOE = 6U;

  const auto table =
      std::make_shared<Table>(DECKS_PER_SHOE, DECK_SIZE * 2, 1.5, true);

  auto weakRouter = table->getRouter();
  auto router = weakRouter.lock();

  const auto dealer = std::make_shared<Dealer>(weakRouter, router->requestId(),
                                               table->getNodeId());

  const auto player =
      std::make_shared<TuiPlayer>(weakRouter, table->registerPlayer(), 1000);

  // create input handler
  InputNode inputNode{ 'j', 'k', 'l', ';', weakRouter, router->requestId() };

  // create main views
  TitleView titleView;
  const auto tableView = std::make_shared<TableView>(
      weakRouter, router->requestId(), titleView.getBottomY(),
      1 + bjdim::STATS_WIDTH);

  const auto wagerViewStarty =
      tableView->getTopY() +
      (tableView->getHeight() / 2 - bjdim::WAGER_HEIGHT / 2);
  const auto wagerViewStartx =
      tableView->getLeftX() +
      (tableView->getWidth() / 2 - bjdim::WAGER_WIDTH / 2);

  const auto wagerView = std::make_shared<WagerView>(
      weakRouter, router->requestId(), wagerViewStarty, wagerViewStartx);

  const auto statsView =
      std::make_shared<StatsView>(weakRouter, router->requestId(), 1, 1);

  drawViewsToScreen();
  const auto controllerNode = router->requestId();

  EventHandler<void, DrawToScreenCmd> drawCmdHandler =
      [](const WrappedEvent<void, DrawToScreenCmd>& e) {
        (void)e;
        drawViewsToScreen();
      };

  EventHandler<void, CardResp> onCardDrawnHandler =
      [controllerNode](const WrappedEvent<void, CardResp>& e) {
        e.router.broadcast(controllerNode, ViewUpdateCmd<void>{});
        drawViewsToScreen();
      };

  router->listen(controllerNode, false, drawCmdHandler);
  // update all the views every time a card is drawn
  router->listen(controllerNode, true, onCardDrawnHandler);

  while (true) {
    router->invoke(controllerNode, dealer->getNodeId(), StartRoundCmd<void>{});

    // TODO: move to tui controller
    router->broadcast(controllerNode,
                      TableViewDealerDownCardVisCmd<void>{ false });
    router->broadcast(controllerNode, ViewUpdateCmd<void>{});
    drawViewsToScreen();

    // TODO: offer insurance
    const auto dealerHasBlackjack = dealer->checkDealerBlackjack();
    if (!dealerHasBlackjack) {
      while (dealer->getCurrPlayerNode() != table->getEndPlayer()) {
        router->broadcast(controllerNode, InputGetAndMapKeyPress<void>{});
        router->broadcast(controllerNode, ViewUpdateCmd<void>{});
        drawViewsToScreen();
      }
      router->broadcast(controllerNode, DealerPlayHandCmd<void>{});
    }

    // TODO: fire round results cmd
    dealer->handleRoundResults();

    router->broadcast(controllerNode,
                      TableViewDealerDownCardVisCmd<void>{ true });
    router->broadcast(controllerNode, ViewUpdateCmd<void>{});
    drawViewsToScreen();

    // wait for input so player can see round results
    if (getch() == 'q') {
      break;
    }
  }

  endwin();
  return 0;
}