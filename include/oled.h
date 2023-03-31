#ifndef OLED_DISPLAY
#define OLED_DISPLAY
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#ifdef HELTEC_WIFI_KIT_32
#define OLED_CLOCK 15
#define OLED_DATA 4
#define OLED_RESET 16
#endif

#ifdef HELTEC_WIFI_LORA_32_V3
#define OLED_CLOCK 18
#define OLED_DATA 17
#define OLED_RESET 21
#endif

class OLEDManager {
    public:
        /**
         * Initialize display
         */
        OLEDManager();
        /**
         * Output temperatures to the display
         */
        void render_text(int x, int y, String msg, const uint8_t* font);
        /**
         * Clears display.
         */
        void clear(void);
        /**
         * Sends buffer to display
         */
        void send();
        /**
         * Sets the display to on (if true) or off.
         */
        void set_display(bool on);
        /**
         * Returns true is display is on
         */
        bool is_display_on();
        /**
         * Efficient displaying frequency
         */
        void display_frequency(double freq);

    private:
        bool display_on = false;
        U8G2_SSD1306_128X64_NONAME_F_SW_I2C* u8g2;
};

#endif