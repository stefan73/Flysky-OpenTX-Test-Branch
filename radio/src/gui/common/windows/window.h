/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <list>
#include <touch_driver.h>
#include "bitmapbuffer.h"
#include "debug.h"

class Window {
  public:
    Window(Window * parent, const rect_t & rect):
      parent(parent),
      rect(rect),
      innerWidth(rect.w),
      innerHeight(rect.h)
    {
      if (parent) {
        parent->addChild(this);
      }
    }

    virtual ~Window()
    {
      if (focusWindow == this) {
        focusWindow = nullptr;
      }
    }

    void deleteLater()
    {
      trash.push_back(this);
    }

    void clear()
    {
      scrollPositionX = 0;
      scrollPositionY = 0;
      innerWidth = rect.w;
      innerHeight = rect.h;
      for (auto window: children) {
        window->deleteLater();
      }
      children.clear();
    }

    bool hasFocus()
    {
      return focusWindow == this;
    }

    void clearFocus()
    {
      if (focusWindow) {
        focusWindow->onFocusLost();
      }
      focusWindow = NULL;
    }

    void setFocus()
    {
      clearFocus();
      focusWindow = this;
    }

    void setWidth(coord_t w) {
      rect.w = w;
    }

    void setHeight(coord_t h) {
      rect.h = h;
    }

    void setInnerWidth(coord_t w) {
      innerWidth = w;
    }

    void setInnerHeight(coord_t h) {
      innerHeight = h;
    }

    virtual void paint(BitmapBuffer * dc) { }

    void drawVerticalScrollbar(BitmapBuffer * dc);

    void paintChildren(BitmapBuffer * dc);

    void fullPaint(BitmapBuffer * dc);

    bool pointInRect(coord_t x, coord_t y, rect_t & rect)
    {
      return (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h);
    }

    virtual void onFocusLost() { };

    virtual bool onTouch(coord_t x, coord_t y);

    virtual bool onSlide(coord_t startX, coord_t startY, coord_t slideX, coord_t slideY);

    void adjustInnerHeight();

    coord_t adjustHeight();

    void moveWindowsTop(coord_t y, coord_t delta);

    void addChild(Window * window) {
      children.push_back(window);
    }

  public: // TODO protected later ...
    Window * parent;
    std::list<Window *> children;
    rect_t rect;
    coord_t innerWidth, innerHeight;
    coord_t scrollPositionX = 0;
    coord_t scrollPositionY = 0;
    static Window * focusWindow;
    static std::list<Window *> trash;
};

class MainWindow: public Window {
  public:
    MainWindow():
      Window(nullptr, {0, 0, LCD_W, LCD_H}) {
    }

    void checkEvents();

    void refresh()
    {
      refresh(rect);
    }

    void refresh(rect_t & rect);

  protected:
    void emptyTrash();
};

extern MainWindow mainWindow;

#endif