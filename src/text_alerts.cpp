#include "text_alerts.h"

//revise constructor
TextManager::TextManager() {
    twilio = new Twilio(TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN);
    Serial.println("TextManager initialized - Twilio connected");

    // UTC offset -6 => -6 * 3600 = -21600
    configTime(-21600, 3600, "pool.ntp.org");
}

/**
 * Check current temperature data. Return string format
 */
void TextManager::retrieve_current_time(char *time_buff, int buff_size) {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return;
    }

    strftime(time_buff, buff_size, "%H:%M on %D", &timeinfo);
}

void TextManager::send_text_alert(String msg) {
    String response;
    bool success = twilio->send_message(USER_PHONE_NUMBER, TWILIO_FROM_NUMBER, msg, response);
    if (!success) {
        Serial.println("Failed sending text message!");
    }
}

void TextManager::send_triggered_alert() {
    char time_buff[50];
    retrieve_current_time(time_buff, 50);
    send_text_alert(String("Critical Safety Event at ") + String(time_buff));
}

void TextManager::send_untriggered_alert() {
    char time_buff[50];
    retrieve_current_time(time_buff, 50);
    send_text_alert(String(time_buff));
}
