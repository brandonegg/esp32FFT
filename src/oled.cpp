#include "oled.h"

OLEDManager::OLEDManager() {
  u8g2 = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, OLED_CLOCK, OLED_DATA, OLED_RESET); //esp32

  u8g2->begin();
  u8g2->setAutoPageClear(true);
  u8g2->setFont(u8g2_font_t0_22b_mf);
  u8g2->setDrawColor(1);
  u8g2->setBitmapMode(0);
  u8g2->setFontMode(0); // Not transparent;
}

void OLEDManager::send() {
  u8g2->sendBuffer();
}

void OLEDManager::render_text(int x, int y, String msg, const uint8_t* font) {
  if (!display_on) {
    return;
  }
  
  u8g2->setFont(font);
  u8g2->drawStr(x, y, msg.c_str());	// write something to the internal memory
}

void OLEDManager::display_frequency(double freq) {
  if (!display_on) {
    return;
  }

  String row2 = String(freq) + " Hz";
  u8g2->setFontMode(0); // Not transparent;
  u8g2->setFont(u8g2_font_chargen_92_mf);
  u8g2->setCursor(0, 48);
  u8g2->print(row2.c_str());
  send();
}

void OLEDManager::clear() {
  u8g2->clearDisplay();
}

void OLEDManager::set_display(bool on) {
  if (!on) {
    clear();
  }
  display_on = on;
}
