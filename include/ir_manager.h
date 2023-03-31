#ifndef IR_MANAGER
#define IR_MANAGER

#include <Arduino.h>

#define ADC_IR_PIN 1 // Pin the IR is plugged into on esp32, must be ADC1 capable
#define SAMPLE_PERIOD 500 //2000 Hz Once every 500 micro seconds.
// Note: Sample period must be 2 times the target frequency. In our case we are targeting 500Hz
// so we want a frequency of 1000Hz which is a period of 0.001 seconds (or 1000 micro seconds)
#define SAMPLE_N 256 //2048 // Must be a power of 2 (FFT_N)

class IRManager {
    public:

        /**
         * Initializes timer and ADC sampling
         */
        IRManager();

        /**
         * Returns the last FFT frequency detected from sample
         * buffer
         */
        double detected_frequency();

        /**
         * Samples the current ADC reading
         */
        void sample();

        /**
         * Returns current buffer index.
         */
        uint16_t get_buffer_index();

        /**
         * Return true if buffer is full (and ready to compute FFT!)
         */
        bool is_buffer_full();

        /**
         * Computes and stores frequency from FFT, computed with buffer.
         * Should only be called if is_buffer_full() returns true. Resets the buffer
         * after calc complete.
         */
        double calc_freq();

    private:
        /**
         * Compute the FFT based on current adc_buffer
         */
        void compute_fft();
        /**
         * Converts ADC current val
         */
        double read_adc_voltage();
        /**
         * Return raw ADC value
         */
        uint16_t read_adc();

        double frequency = 0;
        double amplitude = 0;
        // total duration of the sample collection is # samples / sampling frequency
        //float capture_duration = 3.413; //(float)SAMPLE_N/(float)SAMPLE_PERIOD; //2.048;
        float capture_duration = (float)SAMPLE_N / (1.0 / ((float)SAMPLE_PERIOD / 1000000.0));
        uint16_t buff_index = 0;

        double adc_buffer[SAMPLE_N] = { 0 };
};

#endif