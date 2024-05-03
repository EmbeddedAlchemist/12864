#include <ArduinoJson.hpp>
#include <HTTPClient.h>

#include "GuetWifiLoginer.hpp"

#include "../Display/Console.hpp"
#include "base64.h"
#include <memory>


bool GuetWifiLoginer::testConnection_() {
    HTTPClient cli;
    cli.begin(connectionTestUrl_);
    int retCode = cli.GET();
    cli.end();
    return retCode == t_http_codes::HTTP_CODE_OK;
}

String GuetWifiLoginer::getUrlParam_(const String &url, const String &name) {
    int questionMarkIndex = url.indexOf('?');
    if (questionMarkIndex == -1)
        return "";
    int nameIndex = url.indexOf(name, questionMarkIndex);
    if (nameIndex == -1)
        return "";
    int equalIndex = url.indexOf('=', nameIndex);
    if (equalIndex == -1)
        return "";
    int andIndex = url.indexOf('&', equalIndex);
    int ltIndex = url.indexOf('<', equalIndex);
    int endIndex = andIndex != -1  ? andIndex
                   : ltIndex != -1 ? ltIndex
                                   : url.length() - 1;
    return url.substring(equalIndex + 1, endIndex);
}

WifiLoginer::LoginResult GuetWifiLoginer::login(LoginParam &param) {
    LoginResult result;
    result.retry = true;
    result.success = false;
    result.retryDelay = 1000;
    HTTPClient cli;

    // check for network connection
    // if we can access internet, then skip authentication
    Console.println("Checking network...");
    bool connTestResult = testConnection_();
    if (connTestResult == true) {
        Console.println("Network avaliable, skip login.");
        result.success = true;
        return result;
    }
    Console.println("No connection.");
    cli.begin(connectionTestUrl_);
    cli.GET();
    String redirect = cli.getString();
    cli.end();

    // get params from redirect url

    // remove dashes from mac
    auto formatMac = [](String inp) -> String {
        if (inp.length() != 17)
            return emptyString;
        char ret[] = {
            inp[0], inp[1],
            inp[3], inp[4],
            inp[6], inp[7],
            inp[9], inp[10],
            inp[12], inp[13],
            inp[15], inp[16],
            '\0'};
        return ret;
    };

    String account = userName_;
    String password = base64::encode(password_);
    String userIp = getUrlParam_(redirect, "wlanuserip");
    String acName = getUrlParam_(redirect, "wlanacname");
    String acIp = getUrlParam_(redirect, "wlanacip");
    String userMac = formatMac(getUrlParam_(redirect, "wlanusermac"));
    Console.println("Login to GUET-WiFi with:");
    Console.print("Account: "), Console.println(account);
    Console.print("Password: "), Console.println(password);
    // Console.print("UserIP: "), Console.println(userIp);
    // Console.print("UserMAC: "), Console.println(userMac);
    // Console.print("ACIP: "), Console.println(acIp);
    // Console.print("ACName: "), Console.println(acName);

    // format uri
    std::size_t loginUriSize =
        strlen(loginUriFormat_) +
        account.length() +
        password.length() +
        userIp.length() +
        acName.length() +
        acIp.length() +
        userMac.length() +
        32;
    auto loginUri = std::unique_ptr<char[]>(new char[loginUriSize]);
    snprintf(
        loginUri.get(),
        loginUriSize,
        loginUriFormat_,
        "_",
        "1",
        account.c_str(),
        password.c_str(),
        userIp.c_str(),
        "",
        userMac.c_str(),
        acIp.c_str(),
        acName.c_str());

    Serial.println(loginUri.get());

    cli.begin(loginHost_, loginPort_, String(loginUri.get()));
    int code = cli.GET();
    if (code != 200) {
        cli.end();
        Console.print("Login failed with code "), Console.println(code);
        return result;
    }
    auto &resultStream = cli.getStream();
    for (;;) {
        int c = resultStream.read();
        if (c < 0 || c == '(')
            break;
    }
    ArduinoJson::JsonDocument jDoc;
    auto err = ArduinoJson::deserializeJson(jDoc, resultStream);
    cli.end();
    if (err != ArduinoJson::DeserializationError::Ok) {
        Console.println(err.c_str());
        return result;
    }
    if (!jDoc.containsKey("result")) {
        Console.println("result key not found");
        return result;
    }
    if (!jDoc["result"].is<int>()) {
        Console.println("result type error");
        return result;
    }
    int jRes = jDoc["result"].as<int>();
    if(jRes != 1){
        Console.println("result should be 1");
        return result;
    }
    Console.println("Check network access");
    connTestResult = testConnection_();
    if(connTestResult != true){
        Console.println("No network access");
        return result;
    }
    result.success = true;
    result.retry = false;
    result.retryDelay = 1000;
    return result;
}

GuetWifiLoginer::GuetWifiLoginer(String userName, String password)
    : userName_(userName), password_(password) {
}
