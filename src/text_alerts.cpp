#include "text_alerts.h"

/**
 * Check current temperature data. Return string format
 */
String get_current_time() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

//revise constructor
TextManager::TextManager() {
    twilio = new Twilio(TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN);
    Serial.println("TextManager initialized - Twilio connected");
    bool prev_outside_range = false;
}

PhoneAlertData* TextManager::get_data() {
    return phone_data;
}

//plan to remove this
/*
void TextManager::update_data(String phone_number, float min_temp, float max_temp, char unit) {
    phone_data->phone_number = phone_number;
    phone_data->min_temp = min_temp;
    phone_data->max_temp = max_temp;
    phone_data->unit = unit;
}
*/

void TextManager::update_data(String hour, String minute, String month, String day) {
    phone_data->hour = hour;
    phone_data->minute = minute;
    phone_data->month = month;
    phone_data->day = day;
}


void TextManager::send_text_alert(String msg) {
    String response;
    bool success = twilio->send_message(phone_data->phone_number, TWILIO_FROM_NUMBER, msg, response);
    if (!success) {
        Serial.println("Failed sending text message!");
    }
}

void TextManager::send_triggered_alert() {

}

void TextManager::send_untriggered_alert() {

}
