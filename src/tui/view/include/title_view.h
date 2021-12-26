#include "tui.h"
#include "view.h"

class TitleView : public View {
public:
  TitleView() : View(bjdim::TITLE_HEIGHT, COLS, 0, 0) {
    wattron(window, COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
    wbkgd(window, COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
    wprintw(window, "Blackjack");
  }

  virtual void update() override {}
};