#include "view.h"
#include "strfmt.h"
#include <cstddef>
#include <memory>
#include <ncurses.h>
#include <panel.h>
#include <string>

void deleteWindow(WINDOW* window) {
  if (window != nullptr) {
    delwin(window);
  }
}

void deletePanel(PANEL* panel) {
  if (panel != nullptr) {
    del_panel(panel);
  }
}

View::View(int height, int width, int starty, int startx)
    : window(WindowPtr(newwin(height, width, starty, startx), &deleteWindow)),
      panel(PanelPtr(new_panel(window.get()), &deletePanel)), height(height),
      width(width), starty(starty), startx(startx) {}

int View::getHeight() const { return height; }

int View::getWidth() const { return width; }

int View::getTopY() const { return starty; }

int View::getLeftX() const { return startx; }

int View::getBottomY() const { return starty + height; }

int View::getRightX() const { return startx + width; }

void View::hide() { hide_panel(panel.get()); }

void View::show() { show_panel(panel.get()); }

void View::sendToFront() { top_panel(panel.get()); }