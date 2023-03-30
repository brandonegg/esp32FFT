#include <Arduino.h>
#include <WiFi.h>
#include "oled.h"
#include "wifi_util.h"
#include "secrets.h"
#include "text_alerts.h"
#include "FFT.h"
#include "FFT_signal.h"

void do_magic() {
  char print_buf[300];
  fft_config_t *real_fft_plan = fft_init(FFT_N, FFT_REAL, FFT_FORWARD, fft_input, fft_output);

  for (int k = 0 ; k < FFT_N ; k++)
    real_fft_plan->input[k] = (float)fft_signal[k];

  long int t1 = micros();
  // Execute transformation
  fft_execute(real_fft_plan);
  
  // Print the output
  for (int k = 1 ; k < real_fft_plan->size / 2 ; k++)
  {
    /*The real part of a magnitude at a frequency is followed by the corresponding imaginary part in the output*/
    float mag = sqrt(pow(real_fft_plan->output[2*k],2) + pow(real_fft_plan->output[2*k+1],2))/1;
    float freq = k*1.0/TOTAL_TIME;
//    sprintf(print_buf,"%f Hz : %f", freq, mag);
//    Serial.println(print_buf);
    if(mag > max_magnitude)
    {
        max_magnitude = mag;
        fundamental_freq = freq;
    }
  }
  long int t2 = micros();
  
  Serial.println();
  /*Multiply the magnitude of the DC component with (1/FFT_N) to obtain the DC component*/
  sprintf(print_buf,"DC component : %f g\n", (real_fft_plan->output[0])/10000/FFT_N);  // DC is at [0]
  Serial.println(print_buf);

  /*Multiply the magnitude at all other frequencies with (2/FFT_N) to obtain the amplitude at that frequency*/
  sprintf(print_buf,"Fundamental Freq : %f Hz\t Mag: %f g\n", fundamental_freq, (max_magnitude/10000)*2/FFT_N);
  Serial.println(print_buf);

  Serial.print("Time taken: ");Serial.print((t2-t1)*1.0/1000);Serial.println(" milliseconds!");
  
  // Clean up at the end to free the memory allocated
  fft_destroy(real_fft_plan);
}

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

  Serial.println("Starting FFT");
  do_magic();
}

void loop() {

}
