#pragma once

#include <U8g2lib.h>

class DisplayHandler {
  protected:
    friend class _DisplayManager;
    virtual void onDraw(U8G2 &display) = 0;
};
