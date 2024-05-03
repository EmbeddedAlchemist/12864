#pragma once

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Ticker.h>

#include <freertos/task.h>
#include <freertos/semphr.h>

class _TimeSync {
    static constexpr const char *ntpServer_ = "ntp.ntsc.ac.cn";

  private:
    WiFiUDP udp_;
    NTPClient cli_;
    Ticker ticker_;

    StaticTask_t taskBuffer_;
    StackType_t taskStack_[1024*2];
    TaskHandle_t taskHandle_;

    void tickerHandler_();
    void taskFunction_();

  public:
    void sync();
    void startAutoSync(std::uint32_t intervalMs);
    void stopAutoSync();

    _TimeSync();
};

extern _TimeSync TimeSync;