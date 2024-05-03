#include "TimeSync.hpp"
#include "../Display/Console.hpp"
#include "WifiManager.hpp"
#include <freertos/portmacro.h>

void _TimeSync::tickerHandler_() {
    sync();
}

void _TimeSync::taskFunction_() {
    for (;;) {
        xTaskNotifyWait(0, 0, nullptr, portMAX_DELAY);
        WifiManager.requestAndWait();
        cli_.begin();
        bool success = false;
        for (int retry = 0;; retry++) {
            if (retry == 10) {
                Console.println("Time sync Failed");
                break;
            }
            if (cli_.forceUpdate()) {
                success = true;
                break;
            }
        }
        if (success) {
            timeval _timeval;
            timezone _timezone;
            cli_.setTimeOffset(60 * 60 * 8);
            _timeval.tv_sec = cli_.getEpochTime();
            _timeval.tv_usec = 0;
            _timezone.tz_dsttime = DST_NONE;
            _timezone.tz_minuteswest = 0;
            settimeofday(&_timeval, &_timezone);
            Console.println("Time sync success");
        }
        cli_.end();
        WifiManager.release();
    }
}

void _TimeSync::sync() {
    if (xPortInIsrContext() == pdTRUE) {
        Serial.println("In interrupt");
        BaseType_t higherPriorityTaskWoken;
        xTaskNotifyFromISR(taskHandle_, 0, eNotifyAction::eNoAction, &higherPriorityTaskWoken);
    } else {
        Serial.println("Not in interrupt");
        xTaskNotify(taskHandle_, 0, eNotifyAction::eNoAction);
    }
}

void _TimeSync::startAutoSync(std::uint32_t intervalMs) {
    ticker_.attach_ms(
        intervalMs,
        (void (*)(_TimeSync *))[](_TimeSync * _this) { _this->tickerHandler_(); },
        this);
}

void _TimeSync::stopAutoSync() {
    ticker_.detach();
}

_TimeSync::_TimeSync()
    : cli_(udp_, ntpServer_) {
    taskHandle_ = xTaskCreateStatic(
        [](void *this_) { ((_TimeSync *)this_)->taskFunction_(); },
        "TimeSync",
        sizeof(taskStack_),
        this,
        1,
        taskStack_,
        &taskBuffer_);
}

_TimeSync TimeSync;