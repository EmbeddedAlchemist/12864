#pragma once

#include <cstdint>

class WifiLoginer {
  protected:
    friend class _WifiManager;
    struct LoginResult {
        bool success = true;
        bool retry = false;
        std::uint32_t retryDelay = 0;
    };

    struct LoginParam {
        std::uint32_t retryCount;
    };

    inline virtual LoginResult login(LoginParam &) { return LoginResult(); };
};