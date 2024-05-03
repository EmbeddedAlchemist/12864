#line 1 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\src\\Display\\DisplayHandler.hpp"
#pragma once

#include <U8g2lib.h>

class DisplayHandler {
  protected:
    friend class _DisplayManager;
    virtual void onDraw(U8G2 &display) = 0;
};
