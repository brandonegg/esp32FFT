#include "ir_manager.h"
#include "FFT.h"

float fft_input[SAMPLE_N];
float fft_output[SAMPLE_N];

void IRManager::compute_fft() {
  float max_magnitude = 0;
  float fundamental_freq = 0;

  //char print_buf[300];
  fft_config_t *real_fft_plan = fft_init(SAMPLE_N, FFT_REAL, FFT_FORWARD, fft_input, fft_output);

  for (int k = 0 ; k < SAMPLE_N ; k++)
    real_fft_plan->input[k] = (float)adc_buffer[k]; // fft_signal

  // Execute transformation
  fft_execute(real_fft_plan);
  
  // Print the output
  for (int k = 1 ; k < real_fft_plan->size / 2 ; k++)
  {
    /*The real part of a magnitude at a frequency is followed by the corresponding imaginary part in the output*/
    float mag = sqrt(pow(real_fft_plan->output[2*k],2) + pow(real_fft_plan->output[2*k+1],2))/1;
    float freq = k*1.0/capture_duration; // Old = TOTAL_TIME

    if(mag > max_magnitude)
    {
        max_magnitude = mag;
        fundamental_freq = freq;
    }
  }

  magnitude = (max_magnitude/10000)*2/SAMPLE_N;
  frequency = fundamental_freq;

  // Clean up at the end to free the memory allocated
  fft_destroy(real_fft_plan);
}

uint16_t IRManager::read_adc() {
  return analogRead(ADC_IR_PIN);
}

double IRManager::read_adc_voltage() {
  double reading = analogRead(ADC_IR_PIN);
  return (double)reading * (3.3 / 4095.0);
}

IRManager::IRManager() {
    Serial.println("Initializing the IR sampler");

    pinMode(1, INPUT_PULLUP); // enable input as pull-up
    analogReadResolution(12); // set the resolution of the ADC to 12 bits (0-4095)

    Serial.println("IR sampling configured!");
}

uint16_t IRManager::get_buffer_index() {
  return buff_index;
}

bool IRManager::is_buffer_full() {
  return buff_index == SAMPLE_N;
}

double IRManager::calc_freq() {
  compute_fft();

  buff_index = 0;
  return frequency;
}

void IRManager::sample() {
  if (buff_index == SAMPLE_N) {
    // Buffer full!
    return;
  }

  adc_buffer[buff_index] = read_adc();
  buff_index++;
}
