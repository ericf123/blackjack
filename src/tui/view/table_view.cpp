#include "table_view.h"
#include "dealer.h"
#include "hand_view.h"
#include "player.h"
#include "table.h"
#include "tui.h"

TableView::TableView(std::shared_ptr<Table> table,
                     std::shared_ptr<Dealer> dealer, int starty, int startx)
    : View(bjdim::TABLE_HEIGHT, COLS - bjdim::STATS_WIDTH - 2, starty, startx),
      table(table), dealer(dealer) {
  // border
  wattron(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD));
  wbkgd(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD));
  box(window.get(), 0, 0);

  wattron(window.get(), A_BOLD);
  printHCenter(bjdim::HAND_HEIGHT + 1, "Blackjack Pays %0.2f",
               table->getBlackjackPayoutRatio());
  if (table->shouldDealerHitSoft17()) {
    printHCenter(bjdim::HAND_HEIGHT + 2, "Dealer Must Hit Soft 17s");
  } else {
    printHCenter(bjdim::HAND_HEIGHT + 2, "Dealer Stands on All 17s");
  }
  wattroff(window.get(), A_BOLD);

  // layout CardViews that display each card (all initially face down)
  using HandArray = std::array<HandView, MAX_PLAYERS + 1>;

  const auto handStartY = getBottomY() - bjdim::HAND_HEIGHT - 1;
  handViews = std::make_unique<HandArray>(HandArray{
      HandView{ std::nullopt, starty + 1,
                startx + width / 2 - bjdim::HAND_WIDTH / 2 }, // dealer's hand
      HandView{ std::nullopt, handStartY, startx + 1 },
      HandView{ std::nullopt, handStartY, startx + bjdim::HAND_WIDTH + 2 },
      HandView{ std::nullopt, handStartY, startx + bjdim::HAND_WIDTH * 2 + 3 },
      HandView{ std::nullopt, handStartY, startx + bjdim::HAND_WIDTH * 3 + 4 },
      HandView{ std::nullopt, handStartY, startx + bjdim::HAND_WIDTH * 4 + 5 },
  });

  setDealerUpCardVisible(false);
  draw();
}

void TableView::update() { draw(); }

void TableView::setDealerUpCardVisible(bool visibility) {
  (*handViews.get())[DEALER_HAND_INDEX].setFirstCardVisible(visibility);
}

void TableView::drawIndividualHand(std::optional<ConstHandIter> beginHand,
                                   std::optional<ConstHandIter> currHand,
                                   std::optional<ConstHandIter> endHand,
                                   size_t index) {
  if (currHand) {
    auto& handView = (*handViews.get())[index];
    handView.setHandRange(beginHand, currHand, endHand);
    handView.update();
    handView.show();
  }
}

void TableView::draw() {
  // first hand view is the dealers
  drawIndividualHand(std::nullopt, dealer->getDealerHand(), std::nullopt,
                     DEALER_HAND_INDEX);

  auto playerIter = table->getBeginPlayer();
  for (auto i = 1U; i < MAX_PLAYERS + 1; ++i) {
    if (playerIter != table->getEndPlayer()) {
      const auto& player = *playerIter;
      const auto hand = player->getCurrentHand();

      if (hand) {
        drawIndividualHand(player->getBeginHand(), hand, player->getEndHand(),
                           i);
      }

      ++playerIter;
    } else {
      (*handViews.get())[i].hide();
    }
  }
}