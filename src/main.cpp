#include <Arduino.h>
#include <WiFi.h>
#include "oled.h"
#include "wifi_util.h"
#include "secrets.h"
#include "text_alerts.h"
#include "FFT.h"
#include "FFT_signal.h"

#define SAMPLE_PERIOD 10 // Once every 1 seconds (time in uS).

void compute_fft() {
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

void timed_adc_sample(void* arg) {
  Serial.println(ReadVoltage(1),3);
  Serial.println(analogRead(1));
  delay(100);
}

void init_adc_sampling() {
  Serial.println("Initializing the ADC");
  pinMode(1, INPUT_PULLUP); // enable input
  analogReadResolution(12); // set the resolution of the ADC to 12 bits (0-4095)

  Serial.println("Initializing sampling routine");

  esp_timer_create_args_t timer_args = {
    .callback = &timed_adc_sample,
    .arg = NULL,
    .name = "temp readings"
  };

  esp_timer_create(&timer_args, &timer);
  esp_timer_start_periodic(timer, TEMPERATURE_POLL_FREQUENCY);
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

  text->send_triggered_alert();

  //compute_fft();

  init_adc_sampling();
}

void loop() {
  
}
