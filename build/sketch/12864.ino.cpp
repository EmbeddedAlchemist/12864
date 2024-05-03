#line 1 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\12864.ino"
#include <Arduino.h>
#include <U8g2lib.h>
#include <cstdint>

#include <cmath>

#include "src/Network/WifiManager.hpp"

#include "src/Display/Console.hpp"
#include "src/Display/DisplayManager.hpp"
#include "src/Display/MainUi.hpp"

#include "src/Network/GuetWifiLoginer.hpp"
#include "src/Network/TimeSync.hpp"

#include "src/BSP/Pins.hpp"

#include <HTTPClient.h>
#include <NTPClient.h>
#include <Ticker.h>
#include <functional>



class PhyButton {
    static constexpr std::uint32_t longPressTime_ = 300;
    static constexpr std::uint32_t ignoreTime_ = 5;

  public:
    using CallbackFunction_t = void(void *);

  private:
    static void defaultHandler_(void *) {}
    std::uint8_t pin_;
    Ticker ticker_;
    std::uint32_t lastTriggerTime_;
    bool tickerTriggered_;
    std::function<CallbackFunction_t>
        keyDownCallback_ = defaultHandler_,
        keyUpCallback_ = defaultHandler_,
        keyPressCallback_ = defaultHandler_,
        keyLongPressCallback_ = defaultHandler_;
    void
        *keyDownParam_ = nullptr,
        *keyUpParam_ = nullptr,
        *keyPressParam_ = nullptr,
        *keyLongPressParam_ = nullptr;

    void tickerHandler_() {
        Serial.println("LONGPRESS");

        tickerTriggered_ = true;
        keyLongPressCallback_(keyLongPressParam_);
    }

    void interruptHandler_() {
        if (digitalRead(pin_) == 0) {
            Serial.println("DOWN");
            lastTriggerTime_ = millis();
            ticker_.once_ms(
                longPressTime_,
                (void (*)(PhyButton *))[](PhyButton * _this) { _this->tickerHandler_(); },
                this);
            keyDownCallback_(keyDownParam_);
        } else {
            Serial.println("UP");

            if (millis() - lastTriggerTime_ < ignoreTime_)
                return;
            ticker_.detach();
            if (!tickerTriggered_) {
                Serial.println("PRESS");

                keyPressCallback_(keyPressParam_);
            }
            tickerTriggered_ = false;
            keyUpCallback_(keyUpParam_);
        }
    };

  public:
    PhyButton(std::uint8_t pin) : pin_(pin) {
        attachInterruptArg(
            pin_, [](void *this_) { ((PhyButton *)this_)->interruptHandler_(); }, this, CHANGE);
    };
    void onDown(std::function<CallbackFunction_t> func, void *param = nullptr) {
        keyDownCallback_ = func;
        keyDownParam_ = param;
    }
    void onUp(std::function<CallbackFunction_t> func, void *param = nullptr) {
        keyUpCallback_ = func;
        keyUpParam_ = param;
    }
    void onPress(std::function<CallbackFunction_t> func, void *param = nullptr) {
        keyPressCallback_ = func;
        keyPressParam_ = param;
    }
    void onLongPress(std::function<CallbackFunction_t> func, void *param = nullptr) {
        keyLongPressCallback_ = func;
        keyLongPressParam_ = param;
    }
};

U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R2, Pins::LCD_CS, Pins::LCD_RESET);

RTC_NOINIT_ATTR int bootFailCount = 0;

PhyButton button0(Pins::BTN_0), button1(Pins::BTN_1);

WiFiUDP ntpUdp;
NTPClient ntpClient(ntpUdp, "ntp1.aliyun.com");

TaskHandle_t ntpTask = nullptr;

auto timeSyncTask(void *unused) -> void {
    Console.println("Trying to sync time");
    WifiManager.requestAndWait();
    Console.println("Got network");
    ntpClient.begin();
    while (ntpClient.forceUpdate() != true) {
        Console.println("Fail to sync, retrying...");
    }
    String t = ntpClient.getFormattedTime();
    Console.println(t);
    WifiManager.release();
    vTaskDelete(nullptr);
};

#line 129 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\12864.ino"
void startTimeSync();
#line 140 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\12864.ino"
float getBattLevelByVbat(float vbat);
#line 144 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\12864.ino"
void enterDeepSleep();
#line 173 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\12864.ino"
void u8g2Init();
#line 183 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\12864.ino"
void setup();
#line 237 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\12864.ino"
void loop();
#line 129 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\12864.ino"
void startTimeSync() {
    if (ntpTask != nullptr)
        return;
    xTaskCreate(timeSyncTask,
                "ntp",
                4096,
                nullptr,
                1,
                &ntpTask);
}

float getBattLevelByVbat(float vbat) {
    return (1.02780325852588f - -1.76050625146388E-05f) / (1 + std::pow((vbat / 3.84655624953406f), -41.4361734808687f)) + -1.76050625146388E-05f;
}

void enterDeepSleep() {
    // wait for all network transactions finish
    DisplayManager.setHandler(Console);
    Console.println("Shuting down...");
    Console.println("Wait for network task finish...");
    WifiManager.waitForState(WifiState::Off);

    delay(500);
    DisplayManager.end();
    detachInterrupt(Pins::BTN_0);
    digitalWrite(Pins::BACKLIGHT_CONTROL, LOW);
    digitalWrite(Pins::LCD_CONTROL, LOW);
    gpio_hold_en(gpio_num_t::GPIO_NUM_4);
    gpio_hold_en(gpio_num_t::GPIO_NUM_5);
    gpio_deep_sleep_hold_en();
    gpio_set_direction(gpio_num_t::GPIO_NUM_0, gpio_mode_t::GPIO_MODE_INPUT);
    esp_deep_sleep_enable_gpio_wakeup(GPIO_SEL_0 | GPIO_SEL_2, esp_deepsleep_gpio_wake_up_mode_t::ESP_GPIO_WAKEUP_GPIO_LOW);
    // gpio_deep_sleep_wakeup_enable(gpio_num_t::GPIO_NUM_0, gpio_int_type_t::GPIO_INTR_NEGEDGE);
    esp_deep_sleep_disable_rom_logging();
    // taskDISABLE_INTERRUPTS();
    // vTaskEndScheduler();
    bootFailCount = 0;
    esp_deep_sleep_start();
    ESP.restart();
}

bool flagBtn0Pressed = false;
bool flagBtn1Pressed = false;

void u8g2Init() {
    u8x8_SetPin(u8g2.getU8x8(), U8X8_PIN_SPI_CLOCK, Pins::LCD_CLK);
    u8x8_SetPin(u8g2.getU8x8(), U8X8_PIN_SPI_DATA, Pins::LCD_DIN);
    // SPI.setFrequency(100000);
    u8g2.setBusClock(400000);
    u8g2.begin();
}

GuetWifiLoginer wifiLoginer("2100301417", "wppepewuL16");

void setup() {

    bootFailCount++;
    if (bootFailCount > 5)
        bootFailCount = 0;

    Serial.begin(115200);
    Pins::init();
    u8g2Init();
    DisplayManager.setHandler(Console);
    DisplayManager.begin(u8g2);
    Console.println("Initialize...");

    if (bootFailCount >= 5) {
        bootFailCount = 0;
        Console.println("Boot Failed!");
        Console.println("[BTN0] Restart");
        Console.println("[BTN1] Power OFF");
        while (true) {
            if (digitalRead(Pins::BTN_0) == 0) {
                bootFailCount = 0;
                ESP.restart();
            }
            if (digitalRead(Pins::BTN_1) == 0) {
                enterDeepSleep();
            }
            taskYIELD();
        }
    }

    WifiManager.begin("GUET-WiFi", "", wifiLoginer);
    TimeSync.sync();
    TimeSync.startAutoSync(1000 * 60 * 1);
    // Console.println(String("Failed count: ") + bootFailCount);

    // attachInterrupt(
    //     Pins::BTN_0, []() { flagBtn0Pressed = true; }, FALLING);
    // attachInterrupt(
    //     Pins::BTN_1, []() { flagBtn1Pressed = true; }, FALLING);

    button0.onPress(
        [](void *) { DisplayManager.setHandler(Console); });

    button0.onLongPress(
        [](void *) { enterDeepSleep(); });

    button1.onPress(
        [](void *) { digitalWrite(Pins::BACKLIGHT_CONTROL, !digitalRead(Pins::BACKLIGHT_CONTROL)); });


    Console.println("BEGIN!");
    DisplayManager.setHandler(MainUi);
}

void loop() {

    // if (flagBtn0Pressed)
    //     flagBtn0Pressed = false, enterDeepSleep();
    // if (flagBtn1Pressed)
    //     flagBtn1Pressed = false, digitalWrite(Pins::BACKLIGHT_CONTROL, !digitalRead(Pins::BACKLIGHT_CONTROL));
}
