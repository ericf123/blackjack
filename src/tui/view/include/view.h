#pragma once

#include <initializer_list>
#include <memory>
#include <cstddef>
#include <ncurses.h>
#include <panel.h>
#include "strfmt.h"

class View {
public:
  View(int height, int width, int starty, int startx);

  // view is not copyable
  View(const View& view) = delete;
  View& operator=(const View& view) = delete;
  View(View&& view);
  virtual ~View();

  virtual void update() = 0;

  int getHeight();
  int getWidth();  
  int getTopY();
  int getLeftX();
  int getBottomY();
  int getRightX();

  virtual void hide();
  virtual void show();
  void sendToFront();

  template<typename ... Args>
  void printHCenter(int starty, const char* fmt, Args ... args) {
    const auto formatted = strfmt(fmt, args...);
    std::string displayStr { "ERROR" };

    if (formatted) {
      displayStr = formatted.value();
    } 

    const auto startx = width / 2 - displayStr.size() / 2;

    if (startx >= 0) {
      mvwprintw(window, starty, startx, displayStr.c_str());
    }
  }
protected:
  WINDOW* window;
  PANEL* panel;
  int height;
  int width;
  int starty;
  int startx;
};

static inline void drawViewsToScreen() {
  update_panels();
  doupdate();
}