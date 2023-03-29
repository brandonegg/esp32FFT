#ifndef TEXT_MANAGER
#define TEXT_MANAGER
#include <Arduino.h>
#include "secrets.h"
#include "twilio.hpp"
#include "time.h"

#define TWILIO_FROM_NUMBER "+18885719233"

/**
 * Manage sending text alerts to user
 */
class TextManager {
    public:
        /**
         * As TemperatureData object is used to determine current temperature and if
         * it falls outside threshold.
         */
        TextManager();
        /**
         * Sends the triggered alert message to the defined phone number
         */
        void send_triggered_alert();
        /**
         * Sends the untriggered alert message to the defined phone number
         */
        void send_untriggered_alert();

    private:
        Twilio* twilio;
        void send_text_alert(String msg);
};

#endif