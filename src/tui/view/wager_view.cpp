#include <form.h>
#include <limits>
#include "strfmt.h"
#include "view.h"
#include "wager_view.h"

WagerView::WagerView(int starty, int startx) 
    : View(10, 50, starty, startx), prevWager(0) {
  
  wattron(window, COLOR_PAIR(bjcolor::PAIR_BKGD));
  wbkgd(window, COLOR_PAIR(bjcolor::PAIR_BKGD));
  box(window, 0, 0);

  fields[0] = new_field(1, 7, starty + height / 2, startx + width - 7, 0, 0);
  // fields[0] = new_field(1, 7, starty + height, startx + width, 0, 0);
  fields[1] = NULL;
  field_opts_off(fields[0], O_AUTOSKIP);
  field_opts_on(fields[0], O_EDIT);
  field_opts_on(fields[0], O_VISIBLE);
  field_opts_on(fields[0], O_PUBLIC);
  field_opts_on(fields[0], O_BLANK);
  set_field_type(fields[0], TYPE_INTEGER, 0, 0, std::numeric_limits<int>::max);
  set_field_just(fields[0], JUSTIFY_RIGHT);
  set_field_fore(fields[0], COLOR_PAIR(bjcolor::PAIR_BKGD_INV));
  set_field_back(fields[0], COLOR_PAIR(bjcolor::PAIR_BKGD));

  form = new_form(&fields.front());
  int rows, cols; 
  scale_form(form, &rows, &cols);
  set_form_win(form, window);
  post_form(form);
  drawViewsToScreen();
}

Wager WagerView::getWager() {
  wattron(window, A_BLINK);
  box(window, 0, 0);
  wattroff(window, A_BLINK);
  sendToFront();
  keypad(window, true);
  auto prevWagerStr = strfmt("%d", prevWager);

  if (prevWagerStr) {
    set_field_buffer(fields[0], 0, prevWagerStr.value().c_str());
  }
  set_current_field(form, fields[0]);

  form_driver(form, REQ_FIRST_FIELD);
  form_driver(form, REQ_INS_MODE);
  drawViewsToScreen();

  int ch; 
  while ((ch = getch()) != '\n') {
    switch(ch) {
      case KEY_BACKSPACE:
      case 127:
      case '\b':
        form_driver(form, REQ_PREV_CHAR);
        form_driver(form, REQ_DEL_CHAR);
        break;
      default:
        form_driver(form, ch);
        break;
    }

    drawViewsToScreen();
  }
  form_driver(form, REQ_VALIDATION);

  box(window, 0, 0);
  prevWager = atoi(field_buffer(fields[0], 0));
  return prevWager; 
}

WagerView::WagerView(WagerView&& view) : View(std::move(view)) {
  form = view.form;
  fields = view.fields;
  view.form = nullptr;
}

WagerView::~WagerView() {
  if (form != nullptr) {
    unpost_form(form);
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