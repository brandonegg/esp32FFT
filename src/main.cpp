#include <Arduino.h>
#include <WiFi.h> //Wifi Connecting library
#include "oled.h"
#include "wifi_util.h"
#include "secrets.h"
#include "text_alerts.h"
#include <IRremote.h>

IRrecv irrecv(12);
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

  Serial.println("Wifi is connected!");
  oled->set_display(true);
  oled->render_text(0,28, "Wifi connected!", u8g2_font_6x13O_tr);
  oled->render_text(0,48, ip.toString(), u8g2_font_6x13B_tf);
  oled->send();

  delay(5000);
  oled->clear();
  oled->render_text(0,28, "System activated", u8g2_font_6x13B_tf);
  oled->send();

  // Initialize text manager
  text = new TextManager();

  text->send_triggered_alert();

  Serial.println("Enabling IR receiver");
  irrecv.enableIRIn();
}

decode_results results;

void loop() {
  if (irrecv.decode(&results)) {
    if (results.decode_type == 0 && results.rawlen > 1) {
      uint16_t sum = results.rawbuf[0] + results.rawbuf[1];
      Serial.println(sum);
      if (results.rawbuf[0] + results.rawbuf[1] == 500) {
        Serial.println("IR signal detected at 500 Hz");
      }
    }
    irrecv.resume();
  }
}
