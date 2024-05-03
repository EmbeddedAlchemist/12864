#line 1 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\src\\Display\\DisplayManager.cpp"
#include "DisplayManager.hpp"

void _DisplayManager::_task(void) {
    for (;;) {
        taskYIELD();
        if (handler == nullptr && display == nullptr)
            continue;
        display->clearBuffer();
        handler->onDraw(*display);
        display->sendBuffer();
    }
}

void _DisplayManager::begin(U8G2 &disp) {
    display = &disp;
    _taskHandle = xTaskCreateStatic(
        [](void *_this) { reinterpret_cast<_DisplayManager *>(_this)->_task(); },
        "Display",
        sizeof(_stackBuffer) / sizeof(_stackBuffer[0]),
        (void *)this,
        1,
        _stackBuffer,
        &_taskBuffer);
    // xTaskCreate(
    //     [](void *_this) { reinterpret_cast<_DisplayManager *>(_this)->_task(); },
    //     "Display",
    //     4096,
    //     (void *)this,
    //     1,
    //     &_taskHandle);
}

void _DisplayManager::end() {
    vTaskDelete(_taskHandle);
}

void _DisplayManager::setHandler(DisplayHandler &handler) {
    this->handler = &handler;
}

_DisplayManager::_DisplayManager() : handler(nullptr), display(nullptr) {
}

_DisplayManager DisplayManager;