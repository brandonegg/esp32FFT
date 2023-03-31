#include "ir_manager.h"

// void compute_fft() {
//   char print_buf[300];
//   fft_config_t *real_fft_plan = fft_init(FFT_N, FFT_REAL, FFT_FORWARD, fft_input, fft_output);

//   for (int k = 0 ; k < FFT_N ; k++)
//     real_fft_plan->input[k] = (float)fft_signal[k];

//   long int t1 = micros();
//   // Execute transformation
//   fft_execute(real_fft_plan);
  
//   // Print the output
//   for (int k = 1 ; k < real_fft_plan->size / 2 ; k++)
//   {
//     /*The real part of a magnitude at a frequency is followed by the corresponding imaginary part in the output*/
//     float mag = sqrt(pow(real_fft_plan->output[2*k],2) + pow(real_fft_plan->output[2*k+1],2))/1;
//     float freq = k*1.0/TOTAL_TIME;
// //    sprintf(print_buf,"%f Hz : %f", freq, mag);
// //    Serial.println(print_buf);
//     if(mag > max_magnitude)
//     {
//         max_magnitude = mag;
//         fundamental_freq = freq;
//     }
//   }
//   long int t2 = micros();
  
//   Serial.println();
//   /*Multiply the magnitude of the DC component with (1/FFT_N) to obtain the DC component*/
//   sprintf(print_buf,"DC component : %f g\n", (real_fft_plan->output[0])/10000/FFT_N);  // DC is at [0]
//   Serial.println(print_buf);

//   /*Multiply the magnitude at all other frequencies with (2/FFT_N) to obtain the amplitude at that frequency*/
//   sprintf(print_buf,"Fundamental Freq : %f Hz\t Mag: %f g\n", fundamental_freq, (max_magnitude/10000)*2/FFT_N);
//   Serial.println(print_buf);

//   Serial.print("Time taken: ");Serial.print((t2-t1)*1.0/1000);Serial.println(" milliseconds!");
  
//   // Clean up at the end to free the memory allocated
//   fft_destroy(real_fft_plan);
// }

void test(uint64_t start_time, uint64_t end_time) {
  Serial.println("Start time: " + String(start_time) + " - End time: " + String(end_time));
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

    pinMode(1, INPUT_PULLUP); // enable input as pullup (not necessary I think?)
    analogReadResolution(12); // set the resolution of the ADC to 12 bits (0-4095)

    Serial.println("IR sampling configured!");
}

uint16_t IRManager::get_buffer_index() {
  return buff_index;
}

bool IRManager::is_buffer_full() {
  return buff_index == SAMPLE_N;
}

void IRManager::calc_buffer() {
  Serial.println("Buffer cleared!");

  buff_index = 0;
}

void IRManager::sample(int64_t curr_time) {
  if (buff_index == SAMPLE_N) {
    // Buffer full!
    return;
  }

  if (buff_index == 0) {
    buffer_start_time = curr_time;
  } else if (buff_index == SAMPLE_N - 1) {
    buffer_end_time = curr_time;
  }

  adc_buffer[buff_index] = read_adc();
  buff_index++;
}
