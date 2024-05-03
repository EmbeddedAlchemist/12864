#include "MainUi.hpp"

#include "../Network/WifiManager.hpp"
#include "../Power/BatteryManager.hpp"

const char *getWeekdayName(int w) {
    switch (w) {
        case 0:
            return "SUN";
        case 1:
            return "MON";
        case 2:
            return "TUE";
        case 3:
            return "WED";
        case 4:
            return "THU";
        case 5:
            return "FRI";
        case 6:
            return "SAT";
        default:
            return "UNK";
    }
}

const char *getWifiStateName(WifiState state) {
    switch (state) {
        case WifiState::Off:
            return "OFF";
        case WifiState::Connecting:
            return "CNT";
        case WifiState::Logining:
            return "LGN";
        case WifiState::Available:
            return "ON";
        default:
            return "MUL";
    }
}

const char *getBattChargeStateStr(ChargeState state){
    if(state == ChargeState::Charge)
        return "+";
    else
        return "-";
}

void _MainUi::onDraw(U8G2 &disp) {
    time_t t;
    time(&t);
    tm localTime = *localtime(&t);
    // Serial.println("Draw!");
    disp.setFontPosTop();
    int dx = 0;
    int dy = 0;

    disp.setFont(u8g2_font_open_iconic_www_1x_t);
    dx += disp.drawGlyph(dx, dy, 72);
    dx += 2;
    disp.setFont(u8g2_font_helvB08_tf);
    dx += disp.drawStr(dx, dy, getWifiStateName(WifiManager.getState()));
    dx += 4;

    char battState[8];
    snprintf(
        battState, sizeof(battState), "%.0f%%%s",
        BatteryManager.getLevel()*100,
        getBattChargeStateStr(BatteryManager.getState()));
    disp.setFont(u8g2_font_open_iconic_embedded_1x_t);
    dx += disp.drawGlyph(dx, dy, 73);
    dx += 2;
    disp.setFont(u8g2_font_helvB08_tf);
    dx += disp.drawStr(dx, dy, battState);
    dx += 4;

    char date[12];
    snprintf(date, sizeof(date), "%02d/%02d %s", localTime.tm_mon + 1, localTime.tm_mday, getWeekdayName(localTime.tm_wday));
    disp.setFont(u8g2_font_helvB08_tf);
    dx += disp.drawStr(dx, dy, date);

    char timeStr[8];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", localTime.tm_hour, localTime.tm_min);
    disp.setFontPosBaseline();
    disp.setFont(u8g2_font_logisoso42_tn);
    disp.drawStr(0, 59, timeStr);
}

_MainUi MainUi;