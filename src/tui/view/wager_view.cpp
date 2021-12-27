#include "wager_view.h"
#include "strfmt.h"
#include "view.h"
#include <form.h>
#include <limits>

WagerView::WagerView(int starty, int startx)
    : FormView(bjdim::WAGER_HEIGHT, bjdim::WAGER_WIDTH, starty, startx),
      prevWager(0) {
  keypad(window.get(), true); // group function keys

  // set up wager input form
  fields[0] = new_field(1, bjdim::WAGER_INPUT_WIDTH, starty + height / 2,
                        startx + width - bjdim::WAGER_INPUT_WIDTH - 1, 0, 0);
  fields[1] = NULL;

  field_opts_off(fields[0], O_AUTOSKIP);
  field_opts_off(fields[0], O_NULLOK);
  field_opts_on(fields[0], O_EDIT);
  field_opts_on(fields[0], O_VISIBLE);
  field_opts_on(fields[0], O_PUBLIC);
  field_opts_on(fields[0], O_BLANK);
  set_field_just(fields[0], JUSTIFY_RIGHT);
  set_field_fore(fields[0], COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
  set_field_back(fields[0], COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
  set_field_fore(fields[0], A_BOLD);
  setWagerRange(0, std::numeric_limits<Wager>::max());

  form = FormPtr(new_form(&fields[0]), &deleteForm);
  set_form_win(form.get(), window.get());

  // set up static appearance of wager box
  wattron(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
  wbkgd(window.get(), COLOR_PAIR(bjcolor::PAIR_BKGD_INV));

  // wager box always has blinking border
  wattron(window.get(), A_BLINK);
  box(window.get(), 0, 0);
  wattroff(window.get(), A_BLINK);

  wattron(window.get(), A_BOLD);
  mvwprintw(window.get(), height / 2, 1, "Wager: $");
  wattroff(window.get(), A_BOLD);
}

Wager WagerView::getWager() {
  show();
  post_form(form.get());
  drawViewsToScreen(); // redraw or the form will cover the screen

  do {
    // default to previous wager
    auto prevWagerStr = strfmt("%d", prevWager);
    if (prevWagerStr) {
      set_field_buffer(fields[0], 0, prevWagerStr.value().c_str());
    }
    set_current_field(form.get(), fields[0]);

    int ch;
    auto firstDigit = true;

    while ((ch = getch()) != '\n') {
      switch (ch) {
      case KEY_BACKSPACE:
      case 127:
      case '\b':
        form_driver(form.get(), REQ_END_FIELD);
        form_driver(form.get(), REQ_DEL_PREV);
        break;
      default:
        // this makes it impossible to enter negative number
        if (ch >= '0' && ch <= '9') {
          // clear field on the first character that the user types
          if (firstDigit) {
            firstDigit = false;
            form_driver(form.get(), REQ_CLR_FIELD);
          }

          form_driver(form.get(), ch);
        }
        break;
      }
    }
    // validation maybe redundant here due to input restriction
  } while (form_driver(form.get(), REQ_VALIDATION) == E_INVALID_FIELD);

  unpost_form(form.get());
  hide();

  // this should never throw because of validation
  prevWager = std::stoi(field_buffer(fields[0], 0));

  return prevWager;
}

void WagerView::setMaxWager(Wager wager) { setWagerRange(minWager, wager); }

void WagerView::setMinWager(Wager wager) { setWagerRange(wager, maxWager); }

void WagerView::setWagerRange(Wager min, Wager max) {
  minWager = min;
  maxWager = max;
  if (fields[0] != nullptr) {
    set_field_type(fields[0], TYPE_INTEGER, 0, minWager, maxWager);
  }
}