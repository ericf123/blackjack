#include <locale.h>
#include <ncurses.h>
#include <panel.h>
#include "card.h"
#include "card_view.h"
#include "dealer.h"
#include "hand.h"
#include "hand_view.h"
#include "stats_view.h"
#include "table.h"
#include "table_view.h"
#include "title_view.h"
#include "tui.h"
#include "tui_player.h"
#include "wager_view.h"

int main(int argc, char** argv) {
  (void) argc;
  (void) argv;

  initscr();
  cbreak();
  noecho();
  curs_set(0); // hide cursor
  setlocale(LC_ALL, "");

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
  const auto table = std::make_shared<Table>(6, 1.5, true);
  const auto dealer = std::make_shared<Dealer>(table);

  // create main views
  TitleView titleView;
  const auto tableView = std::make_shared<TableView>(table, dealer, titleView.getBottomY(), 1 + bjdim::STATS_WIDTH);
  const auto wagerViewStarty = tableView->getTopY() + (tableView->getHeight() / 2 - bjdim::WAGER_HEIGHT / 2);
  const auto wagerViewStartx = tableView->getLeftX() + (tableView->getWidth() / 2- bjdim::WAGER_WIDTH / 2);
  const auto wagerView = std::make_shared<WagerView>(wagerViewStarty, wagerViewStartx);

  const auto player = std::make_shared<TuiPlayer>(1000, tableView, wagerView);
  dealer->addPlayerToTable(player);

  StatsView statsView { player, 1, 1 };

  drawViewsToScreen();

  while (true) {
    dealer->resetRound();
    dealer->dealInitialCards();

    tableView->setDealerUpCardVisible(false);
    tableView->update();
    drawViewsToScreen();

    // TODO: offer insurance
    const auto dealerHasBlackjack = dealer->checkDealerBlackjack();
    if (!dealerHasBlackjack) {
      dealer->runPlayerActions();
      dealer->playDealerHand();
    } 

    dealer->handleRoundResults();

    tableView->setDealerUpCardVisible(true);
    tableView->update();
    statsView.update();
    drawViewsToScreen();

    getch(); // wait for input so player can see round results
  }

  endwin();
  return 0;
}