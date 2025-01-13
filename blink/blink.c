// ----------------------------------
// Submission for BLT Tech Challenge
// Tahmid Kazi
// Sunday, Jan 12, 2025
// ----------------------------------

#include "pico/stdlib.h"
#include "hardware/timer.h"

// GPIO pins for LEDs
#define LED1_PIN 2
#define LED2_PIN 3

// Global variables
volatile uint16_t led1_counter = 0;  // Simulated 16-bit counter
volatile bool led1_state = false;
volatile bool led2_blinking = false;
volatile int led2_blink_count = 0;
volatile absolute_time_t led2_next_blink_time;

// Callback function for LED1 (simulated 16-bit timer at 65,000)
bool led1_callback(struct repeating_timer *t) {
    led1_counter += 650;  // Increment by 650 µs per tick (faster increments)

    if (led1_counter >= 65000) { // Check for 65,000 threshold
        led1_counter = 0;       // Reset counter to simulate wraparound
        led1_state = !led1_state; // Toggle LED1 state
        gpio_put(LED1_PIN, led1_state); // Set LED1 GPIO
    }
    return true; // Repeat the timer
}

// Callback function for LED2 (3-second timer)
bool led2_callback(struct repeating_timer *t) {
    // Start blinking LED2
    led2_blinking = true;
    led2_blink_count = 0;
    led2_next_blink_time = make_timeout_time_ms(0); // Start immediately
    return true;                                    // Repeat the timer
}

int main() {
    // Initialize the standard I/O
    stdio_init_all();

    // Initialize GPIO for LEDs
    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_put(LED1_PIN, 0); // Ensure LED1 starts OFF

    gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    gpio_put(LED2_PIN, 0); // Ensure LED2 starts OFF

    // Create timers for the LEDs
    struct repeating_timer led1_timer;
    struct repeating_timer led2_timer;

    // Add a repeating timer for LED1 (650 µs increments)
    add_repeating_timer_us(650, led1_callback, NULL, &led1_timer);

    // Add a repeating timer for LED2 (3,000,000 µs or 3 seconds)
    add_repeating_timer_us(3000000, led2_callback, NULL, &led2_timer);

    // Main loop
    while (true) {
        // Handle LED2 blinking (non-blocking)
        if (led2_blinking && absolute_time_diff_us(get_absolute_time(), led2_next_blink_time) <= 0) {
            if (led2_blink_count < 10) { // Blink 5 ON/OFF cycles
                gpio_put(LED2_PIN, led2_blink_count % 2); // Toggle LED2
                led2_blink_count++;
                led2_next_blink_time = make_timeout_time_ms(200); // Set next toggle time
            } else {
                // Stop blinking after 5 blinks and turn LED2 OFF
                gpio_put(LED2_PIN, 0); // Ensure LED2 is OFF
                led2_blinking = false; // Disable further blinking until the next 3-second cycle
            }
        }

        // Prevent CPU idle
        tight_loop_contents();
    }

    return 0;
}
