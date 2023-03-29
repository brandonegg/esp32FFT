#include "text_alerts.h"

/**
 * Check current temperature data. Return string format
 */
String get_current_time() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    char buffer[80];
    strftime(buffer, 80, "%A, %B %d %Y %H:%M:%S", &timeinfo);
    String str(buffer);
    return str;
}

//revise constructor
TextManager::TextManager() {
    twilio = new Twilio(TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN);
    Serial.println("TextManager initialized - Twilio connected");
    bool prev_outside_range = false;
}

void TextManager::send_text_alert(String msg) {
    String response;
    bool success = twilio->send_message(USER_PHONE_NUMBER, TWILIO_FROM_NUMBER, msg, response);
    if (!success) {
        Serial.println("Failed sending text message!");
    }
}

void TextManager::send_triggered_alert() {
    send_text_alert("Testing triggered alert - " + get_current_time());
}

void TextManager::send_untriggered_alert() {
    send_text_alert("Testing untriggered alert - " + get_current_time());
}
