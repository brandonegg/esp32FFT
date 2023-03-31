#include <Arduino.h>
#include <WiFi.h>
#include "oled.h"
#include "wifi_util.h"
#include "secrets.h"
#include "text_alerts.h"
#include "ir_manager.h"

OLEDManager* oled;
TextManager* text;
IRManager* ir;
esp_timer_handle_t timer;

// Pin for showing if transmitter detected or not
#define LED_ALERT_PIN 46

/**
 * Note: Depends on ir object declared before timer being initialized.
 * Callback for esp periodic timer.
 */
void timed_adc_sample(void* arg) {
  ir->sample();
}

void init_ir_sampling() {
  ir = new IRManager();

  esp_timer_create_args_t timer_args = {
    .callback = &timed_adc_sample,
    .arg = NULL,
    .name = "ADC sampling IR signal"
  };

  esp_timer_create(&timer_args, &timer);
  esp_timer_start_periodic(timer, SAMPLE_PERIOD);
}

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

  // ir Receiver config
  init_ir_sampling();
  oled->clear();

  // Setup LED pin
  pinMode(46, OUTPUT);
  digitalWrite(46, 1);

  // Prepare frequency display
  oled->clear();
  oled->render_text(0,28, "Frequency:", u8g2_font_6x13B_tf);
}

// # of times the frequency has been computed
int8_t computed_count = 0;
// True if the frequency was in the valid range
bool prev_valid = false;

/**
 * Determines whether the frequency is valid.
 */
void handle_frequency(double freq) {
  bool valid_freq = (freq > 500) && (freq < 600);

  // Skip the first 5 to make sure it is setup correctly.
  if (computed_count < 5) {
    computed_count++;
    prev_valid = valid_freq;
    return;
  }
  
  if (valid_freq && !prev_valid) {
    digitalWrite(46, 1);
  } else if (!valid_freq && prev_valid) {
    text->send_triggered_alert();
    digitalWrite(46, 0);
  }

  prev_valid = valid_freq;
}

void loop() {
  if (ir->is_buffer_full()) {
    double freq = ir->calc_freq();
    handle_frequency(freq);

    Serial.println("Measured frequency: " + String(freq));

    oled->display_frequency(freq);
    oled->send();
  }
}
