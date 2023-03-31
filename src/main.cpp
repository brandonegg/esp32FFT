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

const int analogPin = 1;  // GPIO1 - ADC1

double ReadVoltage(byte pin){
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading >= 4095) {
    //return 0;
  }
  return (double)reading * (3.3 / 4095.0);
  // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  //return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
}

/**
 * Note: Depends on ir object declared before timer can be
 * initialized!
 */
void timed_adc_sample(void* arg) {
  //Serial.println(ReadVoltage(1),3);
  //Serial.println(analogRead(1));
  //time = &(esp_timer_get_time());
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
