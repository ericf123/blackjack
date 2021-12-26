#include <form.h>
#include <limits>
#include "strfmt.h"
#include "view.h"
#include "wager_view.h"

WagerView::WagerView(int starty, int startx) 
    : View(bjdim::WAGER_HEIGHT, bjdim::WAGER_WIDTH, starty, startx), prevWager(0) {
  keypad(window, true); // group function keys

  // set up wager input form
  fields[0] = new_field(1, bjdim::WAGER_INPUT_WIDTH, starty + height / 2, 
    startx + width - bjdim::WAGER_INPUT_WIDTH - 1, 0, 0);
  fields[1] = NULL;

  field_opts_off(fields[0], O_AUTOSKIP);
  field_opts_on(fields[0], O_EDIT);
  field_opts_on(fields[0], O_VISIBLE);
  field_opts_on(fields[0], O_PUBLIC);
  field_opts_on(fields[0], O_BLANK);
  set_field_type(fields[0], TYPE_INTEGER, 0, 0, std::numeric_limits<int>::max);
  set_field_just(fields[0], JUSTIFY_RIGHT);
  set_field_fore(fields[0], COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
  set_field_back(fields[0], COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
  set_field_fore(fields[0], A_BOLD);

  form = new_form(&fields.front());
  set_form_win(form, window);

  // set up static appearance of wager box
  wattron(window, COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
  wbkgd(window, COLOR_PAIR(bjcolor::PAIR_BKGD_INV));

  // wager box always has blinking border
  wattron(window, A_BLINK);
  box(window, 0, 0);
  wattroff(window, A_BLINK);

  wattron(window, A_BOLD);
  mvwprintw(window, height / 2, 1, "Wager: $");
  wattroff(window, A_BOLD);
}

Wager WagerView::getWager() {
  show();
  post_form(form);
  drawViewsToScreen(); // redraw or the form will cover the screen

  do {
    // default to previous wager
    auto prevWagerStr = strfmt("%d", prevWager);
    if (prevWagerStr) {
      set_field_buffer(fields[0], 0, prevWagerStr.value().c_str());
    }
    set_current_field(form, fields[0]);

    int ch; 
    while ((ch = getch()) != '\n') {
      switch(ch) {
        case KEY_BACKSPACE:
        case 127:
        case '\b':
          form_driver(form, REQ_END_FIELD);
          form_driver(form, REQ_DEL_PREV);
          break;
        default:
          // this makes it impossible to enter negative number
          if (ch >= '0' && ch <= '9') {
            form_driver(form, ch);
          }
          break;
      }
    }
  // validation maybe redundant here due to input restriction
  } while (form_driver(form, REQ_VALIDATION) == E_INVALID_FIELD);

  unpost_form(form);
  hide();

  // this should never throw because of validation
  prevWager = std::stoi(field_buffer(fields[0], 0));

  return prevWager; 
}

WagerView::WagerView(WagerView&& view) : View(std::move(view)) {
  form = view.form;
  fields = view.fields;
  view.form = nullptr;
}

WagerView::~WagerView() {
  if (form != nullptr) {
    free_form(form);
  }
  
  for (auto i = 0; i < NUM_FIELDS; ++i) {
    if (fields[i] != nullptr) {
      free_field(fields[i]);
    }
  }

  if (panel != nullptr) {
    del_panel(panel);
  }
  
  if (window != nullptr) {
    delwin(window);
  }
}