#include "led-task.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

const uint LED_PIN = 25;
uint LED_BLINK_PERIOD_US = 500000;

uint64_t led_ts;
led_state_t led_state;
bool bool_led_state;


void led_task_init(){
    led_ts = 0;
    led_state = LED_STATE_OFF;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    bool_led_state = 0;
    gpio_put(LED_PIN, bool_led_state);
}

void led_task_handle(){
    switch (led_state)
    {
    case LED_STATE_OFF:
        bool_led_state = 0;
        break;
    
    case LED_STATE_ON:
        bool_led_state = 1;
        break;

    case LED_STATE_BLINK:
        if (time_us_64() > led_ts)
        {
            led_ts = time_us_64() + (LED_BLINK_PERIOD_US / 2);
            bool_led_state = 1 - bool_led_state;
        }
        break;

    default:
        break;
    }
    gpio_put(LED_PIN, bool_led_state);
}

void led_task_state_set(led_state_t state){
    led_state = state;
}

void led_task_set_blink_period_ms(uint32_t period_ms){
    LED_BLINK_PERIOD_US = period_ms*1000;
}