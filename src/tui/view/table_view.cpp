#include "table_view.h"
#include "blackjack_events.h"
#include "dealer.h"
#include "hand_view.h"
#include "player.h"
#include "table.h"
#include "tui.h"

TableView::TableView(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
                     int starty, int startx)
    : View(bjdim::TABLE_HEIGHT, COLS - bjdim::STATS_WIDTH - 2, starty, startx),
      router(router), sourceNode(sourceNode) {

  auto blackjackPayoutRatio = 0;
  auto hit17 = true;
  if (auto r = router.lock()) {
    EventHandler<void, TableViewDealerDownCardVisCmd> dealerDownCardVisHandler =
        [this](const WrappedEvent<void, TableViewDealerDownCardVisCmd>& e) {
          setDealerUpCardVisible(e.event.visibile);
        };

    EventHandler<void, ViewUpdateCmd> updateHandler =
        [this](const WrappedEvent<void, ViewUpdateCmd>& e) {
          (void)e;
          draw();
        };

    r->listen(sourceNode, false, dealerDownCardVisHandler);
    r->listen(sourceNode, false, updateHandler);

    auto table = r->invokeFirstAvailable(
        sourceNode, ToConstRefInv<std::reference_wrapper<const Table>>{});
    if (table) {
      const auto& t = table.value().get();
      blackjackPayoutRatio = t.getBlackjackPayoutRatio();
      hit17 = t.shouldDealerHitSoft17();
    }
  }

  // border
  wattron(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD));
  wbkgd(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD));
  box(window.get(), 0, 0);

  wattron(window.get(), A_BOLD);
  printHCenter(bjdim::HAND_HEIGHT + 1, "Blackjack Pays %0.2f",
               blackjackPayoutRatio);
  if (hit17) {
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

void TableView::setDealerUpCardVisible(bool visibile) {
  (*handViews.get())[DEALER_HAND_INDEX].setFirstCardVisible(visibile);
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
  if (auto r = router.lock()) {
    const auto dealerOpt = r->invokeFirstAvailable(
        sourceNode, ToConstRefInv<std::reference_wrapper<const Dealer>>{});
    if (dealerOpt) {
      const auto& dealer = dealerOpt.value().get();
      // first hand view is the dealers
      drawIndividualHand(std::nullopt, dealer.getDealerHand(), std::nullopt,
                         DEALER_HAND_INDEX);
    }

    const auto players = r->broadcast(
        sourceNode, ToConstRefInv<std::reference_wrapper<const Player>>{});
    auto currPlayer = players.cbegin();
    const auto endPlayer = players.cend();

    for (auto i = DEALER_HAND_INDEX + 1; i < MAX_PLAYERS + 1; ++i) {
      if (currPlayer != endPlayer) {
        const auto& player = (*currPlayer).get();
        const auto hand = player.getCurrentHand();

        if (hand) {
          drawIndividualHand(player.getBeginHand(), hand, player.getEndHand(),
                             i);
        }

        ++currPlayer;
      } else {
        (*handViews.get())[i].hide();
      }
    }
  }
}