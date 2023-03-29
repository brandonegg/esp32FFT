#include <Arduino.h>
#include <WiFi.h> //Wifi Connecting library
#include "oled.h"
#include "wifi_util.h"
#include "secrets.h"
#include "text_alerts.h"

OLEDManager* oled;
TextManager* text;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  oled = new OLEDManager();
  oled->set_display(true);
  oled->render_text(0,28, "Connecting to WiFi:", u8g2_font_6x13O_tr);
  oled->render_text(0,42, WIFI_SSID, u8g2_font_6x13B_tf);
  oled->send();

  IPAddress ip = connect_wifi();
  oled->clear();
  oled->render_text(0,48, ip.toString(), u8g2_font_6x13B_tf);
  oled->send();

  delay(5000);
  Serial.println("Wifi is connected!");
  oled->set_display(true);
  oled->clear();
  oled->render_text(0,28, "It's connected!", u8g2_font_6x13O_tr);
  oled->send();

  delay(5000);
  oled->set_display(false);

  text->send_triggered_alert();
}

void loop() {
  
}
