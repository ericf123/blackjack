#include <memory>
#include <string>
#include "hand_view.h"

HandView::HandView(const Hand* hand, int starty, int startx) :
  View(bjdim::HAND_HEIGHT, bjdim::HAND_WIDTH, starty, startx), hand(hand), hideFirstCard(false) {
  // border
  wattron(window, COLOR_PAIR(bjcolor::PAIR_BKGD));
  wbkgd(window, COLOR_PAIR(bjcolor::PAIR_BKGD));
  box(window, 0, 0);

  // layout CardViews that display each card (all initially face down)
  using CardArray = std::array<CardView, MAX_CARDS>;

  cardViews = std::make_unique<CardArray>(CardArray {
    CardView { std::nullopt, starty + 1, startx + 1}, 
    CardView { std::nullopt, starty + 1, startx + 1 + bjdim::CARD_WIDTH + 1 },
    CardView { std::nullopt, starty + 1, startx + 1 + bjdim::CARD_WIDTH * 2 + 2 }, 
    CardView { std::nullopt, starty + 1 + bjdim::CARD_HEIGHT + 1, startx + 1 },
    CardView { std::nullopt, starty + 1 + bjdim::CARD_HEIGHT + 1, startx + 1 + bjdim::CARD_WIDTH + 1 },
    CardView { std::nullopt, starty + 1 + bjdim::CARD_HEIGHT + 1, startx + 1 + bjdim::CARD_WIDTH * 2 + 2 },
  });

  renderHand();
}

void HandView::renderHand() {
  auto i = 0;
  if (hand != nullptr) {
    const auto& handToRender = *hand;

    for (auto cardIter = handToRender.getBeginIter(); cardIter != handToRender.getEndIter(); ++cardIter) {
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

    mvwprintw(window, height - 2, 2, "$%d", handToRender.getWager());
  }
}

void HandView::setHand(const Hand* newHand) {
  hand = newHand;
}

void HandView::update() {
  renderHand();
}

void HandView::show() {
  show_panel(panel);
  renderHand();
}

void HandView::hide() {
  auto& viewsArr = *cardViews.get();
  for (auto& view : viewsArr) {
    view.hide();
  }

  hide_panel(panel);
}

void HandView::setFirstCardVisible(bool visible) {
  hideFirstCard = !visible;
}
