#pragma once

#include <cstdint>
#include <Ticker.h>

enum class ChargeState {
    Charge,
    Discharge
};

class _BatteryManager {
  private:
    Ticker ticker_;
    float level_;

    std::uint8_t pinChargeState_, pinVbatScene_;

    void tickerHandler_();

  public:

    float getLevel();
    ChargeState getState();

    _BatteryManager(std::uint8_t pinChargeState, std::uint8_t pinVbatScene);

};

extern _BatteryManager BatteryManager;