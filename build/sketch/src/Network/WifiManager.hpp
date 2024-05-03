#line 1 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\src\\Network\\WifiManager.hpp"

#pragma once

#include <WiFi.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <functional>

#include "WiFiLoginer.hpp"

enum class WifiState : EventBits_t {
    Off = 1 << 0,
    Connecting = 1 << 1,
    Logining = 1 << 2,
    Available = 1 << 3,

    Any = Off | Connecting | Logining | Available,

    Connected = Logining,
    Logined = Available
};

class _WifiManager {
  private:
    static WifiLoginer _defaultLoginer;

  private:
    StaticTask_t _managerTaskBuffer;
    StackType_t _managerTaskStackBuffer[1024*4];
    TaskHandle_t _managerTaskHandle;

    StaticEventGroup_t _eventGroupBuffer;
    EventGroupHandle_t _eventGroupHandle;

    StaticSemaphore_t _requestCountingBuffer, _releaseCountingBuffer;
    SemaphoreHandle_t _requestCountingHandle, _releaseCountingHandle;

    std::uint32_t _usingCount;
    String _ssid, _passprase;

    WifiLoginer *_loginer;

    WifiState _state;

    void _managerTask();
    bool _wifiConnect();
    bool _wifiLogin();
    void _wifiDisconnect();
    void _setState(WifiState state);

  public:
    void begin(String ssid, String passprase = "", WifiLoginer &loginer = _WifiManager::_defaultLoginer);
    void end();
    void request();
    void release();
    void waitForState(WifiState state, TickType_t maxTickToWait = portMAX_DELAY);
    void requestAndWait(TickType_t maxTickToWait = portMAX_DELAY);

    WifiState getState();

    _WifiManager();
};

extern _WifiManager WifiManager;
