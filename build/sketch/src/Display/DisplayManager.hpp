#line 1 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\src\\Display\\DisplayManager.hpp"
#pragma once

#include "DisplayHandler.hpp"
#include <FreeRTOS/task.h>

class _DisplayManager {
  private:
    DisplayHandler *handler;
    U8G2 *display;

    StaticTask_t _taskBuffer;
    StackType_t _stackBuffer[1024*8];
    TaskHandle_t _taskHandle;

    void _task(void);

  public:
    void setHandler(DisplayHandler &handler);
    void begin(U8G2 &disp);
    void end();

    _DisplayManager();
};

extern _DisplayManager DisplayManager;