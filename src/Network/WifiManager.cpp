#include "WifiManager.hpp"
#include <limits>

#include "../Display/Console.hpp"

WifiLoginer _WifiManager::_defaultLoginer = WifiLoginer();

void _WifiManager::_managerTask() {
    for (;;) {
        _usingCount = 0;
        _setState(WifiState::Off);
        // Waiting For Req
        while (xSemaphoreTake(_requestCountingHandle, portMAX_DELAY) == pdTRUE) {
            _usingCount++;
            break;
        }
        _setState(WifiState::Connecting);
        _wifiConnect();

        _setState(WifiState::Logining);
        _wifiLogin();

        // Take all requests
        while (xSemaphoreTake(_requestCountingHandle, 0) == pdTRUE) {
            _usingCount++;
        }

        // it will wake up all task that waiting for wifi connection
        _setState(WifiState::Available);

        // wait for all release
        while (_usingCount > 0) {
            xSemaphoreTake(_releaseCountingHandle, portMAX_DELAY);
            _usingCount--;
        }
    }
}

bool _WifiManager::_wifiConnect() {
    Console.println("Connecting to WiFi with:");
    Console.print("SSID: "), Console.println(_ssid);
    Console.print("PassPrase: "), Console.println(_passprase);
    bool res = WiFi.begin(_ssid, _passprase);
    if (res == WL_CONNECT_FAILED) {
        Console.println("Fail!");
        return false;
    }
    Console.println("Waiting to connect...");
    while (!WiFi.isConnected()) {
        taskYIELD();
    }
    return true;
}

bool _WifiManager::_wifiLogin() {
    WifiLoginer::LoginParam param;
    WifiLoginer::LoginResult result;
    param.retryCount = 0;
    for (;;) {
        result = _loginer->login(param);
        if (result.success) {
            Console.println("Login success.");
            break;
        }
        if (result.retry) {
            Console.println(String("Login failed, retry after ") + result.retryDelay);
            delay(result.retryDelay);
        } else {
            Console.println(String("Login failed, No retry."));
            return false;
        }
    }
    return true;
}

void _WifiManager::_wifiDisconnect() {
    WiFi.disconnect(true, true);
    Console.println("WiFi disconnected.");
}

void _WifiManager::_setState(WifiState state) {
    xEventGroupClearBits(_eventGroupHandle, (EventBits_t)WifiState::Any);
    xEventGroupSetBits(_eventGroupHandle, (EventBits_t)state);
    _state = state;
}

void _WifiManager::request() {
    xSemaphoreGive(_requestCountingHandle);
}

void _WifiManager::begin(String ssid, String passprase, WifiLoginer &loginer) {
    _ssid = ssid;
    _passprase = passprase;
    _loginer = &loginer;
    _managerTaskHandle = xTaskCreateStatic(
        [](void *_this) { ((_WifiManager *)_this)->_managerTask(); },
        "WiFiManager",
        sizeof(_managerTaskStackBuffer) / sizeof(_managerTaskStackBuffer[0]),
        (void *)this,
        1,
        _managerTaskStackBuffer,
        &_managerTaskBuffer);
}

void _WifiManager::release() {
    xSemaphoreGive(_releaseCountingHandle);
}

void _WifiManager::end() {
    waitForState(WifiState::Off);
    vTaskDelete(_managerTaskHandle);
}

_WifiManager::_WifiManager() {
    _usingCount = 0;
    _eventGroupHandle = xEventGroupCreateStatic(&_eventGroupBuffer);
    _requestCountingHandle = xSemaphoreCreateCountingStatic(
        std::numeric_limits<UBaseType_t>::max(),
        std::numeric_limits<UBaseType_t>::min(),
        &_requestCountingBuffer);
    _releaseCountingHandle = xSemaphoreCreateCountingStatic(
        std::numeric_limits<UBaseType_t>::max(),
        std::numeric_limits<UBaseType_t>::min(),
        &_releaseCountingBuffer);
    _setState(WifiState::Off);
}

void _WifiManager::waitForState(WifiState state, TickType_t maxTickToWait) {
    xEventGroupWaitBits(
        _eventGroupHandle,
        (EventBits_t)state,
        pdFALSE,
        pdFALSE,
        maxTickToWait);
}

void _WifiManager::requestAndWait(TickType_t maxTickToWait) {
    request();
    waitForState(WifiState::Available, maxTickToWait);
}

WifiState _WifiManager::getState() {
    return _state;
}

_WifiManager WifiManager;
