#include <memory>
#include <cstddef>
#include <ncurses.h>
#include <panel.h>
#include <string>
#include "strfmt.h"
#include "view.h"

View::View(int height, int width, int starty, int startx) 
  : height(height), width(width), starty(starty), startx(startx) {
  window = newwin(height, width, starty, startx);
  panel = new_panel(window);
}


View::View(View&& view) {
  // TODO upate window/panel to be smart pointers so we can use default move
  height = view.height;
  width = view.width;
  starty = view.starty;
  startx = view.startx;
  window = view.window;
  panel = view.panel;
  view.window = nullptr;
  view.panel = nullptr;
}

View::~View() {
  if (panel != nullptr) {
    del_panel(panel);
  }

  if (window != nullptr) {
    delwin(window);
  }
}

int View::getHeight() {
  return height;
}

int View::getWidth() {
  return width;
}

int View::getTopY() {
  return starty;
}

int View::getLeftX() {
  return startx;
}

int View::getBottomY() {
  return starty + height;
}

int View::getRightX() {
  return startx + width;
}

void View::hide() {
  hide_panel(panel);
}

void View::show() {
  show_panel(panel);
}

void View::sendToFront() {
  top_panel(panel);
}