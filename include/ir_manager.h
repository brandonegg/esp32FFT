#ifndef IR_MANAGER
#define IR_MANAGER

#include <Arduino.h>

#define ADC_IR_PIN 1 // Pin the IR is plugged into on esp32, must be ADC1 capable
#define SAMPLE_PERIOD 1000 // Once every 1 seconds (time in uS).
#define SAMPLE_N 2048 // Must be a power of 2 (FFT_N)

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
         * 
         * @param int64_t curr_time Current time (ms) since esp32 boot sample is being taken
         */
        void sample(int64_t curr_time);

        /**
         * Returns current buffer index.
         */
        uint16_t get_buffer_index();

        /**
         * Return true if buffer is full (and ready to compute FFT!)
         */
        bool is_buffer_full();

        void calc_buffer();

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
        uint16_t buff_index = 0;

        int64_t buffer_start_time = 0;
        int64_t buffer_end_time = 0;

        double adc_buffer[SAMPLE_N] = { 0 };
        bool active = false;
};

#endif