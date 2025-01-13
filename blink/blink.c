#include "pico/stdlib.h"
#include "hardware/timer.h"

// GPIO pins for LEDs
#define LED1_PIN 2
#define LED2_PIN 3

// Global variables
volatile uint16_t led1_counter = 0;  // Counter for LED1 (16-bit simulation)
volatile uint32_t led2_timer_counter = 0;  // Counter for LED2 timing
volatile bool led1_state = false;
volatile bool led2_blinking = false;
volatile uint8_t led2_blink_count = 0;

// Callback function for a shared 16-bit timer
bool led_timer_callback(struct repeating_timer *t) {
    // Increment counters
    led1_counter += 650;        // Increment by 650 µs per tick
    led2_timer_counter += 650;  // Increment for LED2

    // Handle LED1 toggle (65,000 µs or 65 ms threshold)
    if (led1_counter >= 65000) { // 65 ms elapsed
        led1_counter = 0;       // Reset counter
        led1_state = !led1_state; // Toggle LED1 state
        gpio_put(LED1_PIN, led1_state); // Update LED1 GPIO
    }

    // Handle LED2 blinking logic
    if (led2_blinking) {
        if (led2_timer_counter >= 200000) { // 200 ms interval for blinking
            led2_timer_counter = 0;  // Reset timer counter
            if (led2_blink_count < 10) { // Blink 5 ON/OFF cycles (10 toggles)
                gpio_put(LED2_PIN, led2_blink_count % 2); // Toggle LED2
                led2_blink_count++;
            } else {
                // Stop blinking after 5 ON/OFF cycles
                led2_blinking = false;
                gpio_put(LED2_PIN, 0); // Ensure LED2 is OFF
            }
        }
    } else {
        // Start blinking LED2 every 3 seconds
        if (led2_timer_counter >= 3000000) { // 3,000,000 µs or 3 seconds elapsed
            led2_timer_counter = 0;  // Reset counter
            led2_blinking = true;    // Start blinking
            led2_blink_count = 0;    // Reset blink count
        }
    }

    return true; // Repeat the timer
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

    // Create a single repeating timer for both LEDs
    struct repeating_timer led_timer;

    // Add a repeating timer with 650 µs increments
    add_repeating_timer_us(650, led_timer_callback, NULL, &led_timer);

    // Main loop
    while (true) {
        // Prevent CPU idle
        tight_loop_contents();
    }

    return 0;
}
