#include "hand_view.h"
#include "player.h"
#include <memory>
#include <string>

HandView::HandView(std::optional<ConstHandIter> hand, int starty, int startx)
    : View(bjdim::HAND_HEIGHT, bjdim::HAND_WIDTH, starty, startx), hand(hand),
      hideFirstCard(false) {
  // border
  wattron(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD));
  wbkgd(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD));
  box(window.get(), 0, 0);

  // layout CardViews that display each card (all initially face down)
  using CardArray = std::array<CardView, MAX_CARDS>;

  cardViews = std::make_unique<CardArray>(CardArray{
      CardView{ std::nullopt, starty + 1, startx + 1 },
      CardView{ std::nullopt, starty + 1, startx + 1 + bjdim::CARD_WIDTH + 1 },
      CardView{ std::nullopt, starty + 1,
                startx + 1 + bjdim::CARD_WIDTH * 2 + 2 },
      CardView{ std::nullopt, starty + 1 + bjdim::CARD_HEIGHT + 1, startx + 1 },
      CardView{ std::nullopt, starty + 1 + bjdim::CARD_HEIGHT + 1,
                startx + 1 + bjdim::CARD_WIDTH + 1 },
      CardView{ std::nullopt, starty + 1 + bjdim::CARD_HEIGHT + 1,
                startx + 1 + bjdim::CARD_WIDTH * 2 + 2 },
  });

  renderHand();
}

void HandView::renderHand() {
  auto i = 0U;
  if (hand) {
    const auto& handToRender = *hand.value();

    for (auto cardIter = handToRender.getBeginIter();
         cardIter != handToRender.getEndIter(); ++cardIter) {
      // silently fail to render hands more than 6 cards in length
      if (i < cardViews->size()) {
        auto& currView = (*cardViews.get())[i];

        if (hideFirstCard && cardIter == handToRender.getBeginIter()) {
          currView.setCard(std::nullopt);
        } else {
          currView.setCard(*cardIter);
        }

        currView.update();
        currView.show();
        currView.sendToFront();
      }
      ++i;
    }

    wattron(window.get(), A_BOLD);
    mvwprintw(window.get(), height - 2, 2, "$       ");
    mvwprintw(window.get(), height - 2, 2, "$%d", handToRender.getWager());
    // TODO: move to TableView?
    if (handToRender.isSplit()) {
      wattron(window.get(), A_BLINK);
      const auto currIndex = std::distance(beginHand.value(), hand.value()) + 1;
      const auto totalHands = std::distance(beginHand.value(), endHand.value());
      mvwprintw(window.get(), height - 2, width - 6, "(%ld/%ld)", currIndex,
                totalHands);
      wattroff(window.get(), A_BLINK);
    } else {
      mvwprintw(window.get(), height - 2, width - 6, "     ");
    }
    wattroff(window.get(), A_BOLD);
  }
}

void HandView::setHandRange(std::optional<ConstHandIter> beginHand,
                            std::optional<ConstHandIter> hand,
                            std::optional<ConstHandIter> endHand) {
  this->beginHand = beginHand;
  this->hand = hand;
  this->endHand = endHand;
}

void HandView::update() { renderHand(); }

void HandView::show() {
  show_panel(panel.get());
  renderHand();
}

void HandView::hide() {
  auto& viewsArr = *cardViews.get();
  for (auto& view : viewsArr) {
    view.hide();
  }

  hide_panel(panel.get());
}

void HandView::setFirstCardVisible(bool visible) { hideFirstCard = !visible; }
