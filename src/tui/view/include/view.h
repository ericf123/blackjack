#pragma once

#include "strfmt.h"
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <ncurses.h>
#include <panel.h>

void deleteWindow(WINDOW *window);
void deletePanel(PANEL *panel);

using WindowPtr = std::unique_ptr<WINDOW, decltype(&deleteWindow)>;
using PanelPtr = std::unique_ptr<PANEL, decltype(&deletePanel)>;

class View {
public:
  View(int height, int width, int starty, int startx);

  // view is not copyable
  View(const View &view) = delete;
  View &operator=(const View &view) = delete;
  View(View &&view) = default;
  virtual ~View() = default;

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

  template <typename... Args>
  void printHCenter(int starty, const char *fmt, Args... args) {
    const auto formatted = strfmt(fmt, args...);
    std::string displayStr{ "ERROR" };

    if (formatted) {
      displayStr = formatted.value();
    }

    const auto startx = width / 2 - displayStr.size() / 2;

    if (startx >= 0) {
      mvwprintw(window.get(), starty, startx, "%s", displayStr.c_str());
    }
  }

protected:
  WindowPtr window;
  PanelPtr panel;
  int height;
  int width;
  int starty;
  int startx;
};

static inline void drawViewsToScreen() {
  update_panels();
  doupdate();
}