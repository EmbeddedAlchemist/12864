#pragma once

#include "WiFiLoginer.hpp"

class GuetWifiLoginer : public WifiLoginer {
  private:
    static constexpr const char *connectionTestUrl_ = "http://httpstat.us/200";
    static constexpr const char *loginHost_ = "10.0.1.5";
    static constexpr std::uint16_t loginPort_ = 801;
    static constexpr const char *loginUriFormat_ =
        "/eportal/portal/login"
        "?callback=%s"
        "&login_method=%s"
        "&user_account=%%2C0%%2C%s"
        "&user_password=%s"
        "&wlan_user_ip=%s"
        "&wlan_user_ipv6=%s"
        "&wlan_user_mac=%s"
        "&wlan_ac_ip=%s"
        "&wlan_ac_name=%s";

    String userName_;
    String password_;



  private:
    bool testConnection_();
    String getUrlParam_(const String &url, const String &name);

  protected:
    virtual LoginResult login(LoginParam &param);

  public:
    GuetWifiLoginer(String userName, String password);
};
