#include "BatteryManager.hpp"
#include "../BSP/Pins.hpp"

void _BatteryManager::tickerHandler_() {
    float realLevel = analogReadMilliVolts(pinVbatScene_) / 1000.f / 0.5f / 4.2f;

    auto state = getState();

    if (state == ChargeState::Charge) {
        if (level_ > realLevel)
            realLevel = level_;
    } else {
        if (level_ < realLevel)
            realLevel = level_;
    }

    level_ = level_ * 0.95f + realLevel * 0.05f;
    if (level_ < 0.f)
        level_ = 0.0f;
    if (level_ > 1.0f)
        level_ = 1.0f;
}

float _BatteryManager::getLevel() {
    return level_;
}

ChargeState _BatteryManager::getState() {
    return digitalRead(pinChargeState_) == 0 ? ChargeState::Charge : ChargeState::Discharge;
}

_BatteryManager::_BatteryManager(std::uint8_t pinChargeState, std::uint8_t pinVbatScene)
    : pinChargeState_(pinChargeState), pinVbatScene_(pinVbatScene) {
    pinMode(pinChargeState_, INPUT_PULLUP);
    pinMode(pinVbatScene_, ANALOG);
    level_ = analogReadMilliVolts(pinVbatScene_) / 1000.f / 0.5f / 4.2f;
    ticker_.attach_ms(
        1000,
        (void (*)(_BatteryManager *))[](_BatteryManager * this_) { this_->tickerHandler_(); },
        this);
}

_BatteryManager BatteryManager(Pins::CHARGE_STATE, Pins::VBAT_SCENE);
