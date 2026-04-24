#include "stdio.h" // стандартная библиотека ввода-вывода
#include "stdlib.h" // стандартная библиотека..?
#include "pico/stdlib.h" // базовое взаимодействие с микроконтроллером
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "adc-task/adc-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"
void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_off_callback(const char* args)
{
    led_task_state_set(LED_STATE_OFF);
	printf("the led is now off");
}

void led_on_callback(const char* args)
{
    led_task_state_set(LED_STATE_ON);
	printf("the led is now on");
}

void led_blink_callback(const char* args)
{
    led_task_state_set(LED_STATE_BLINK);
	printf("the led is now blinking");
}

void led_blink_set_period_ms_callback(const char* args)
{
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);
    if (period_ms == 0){
        printf("blinking period can't be zero, that's just led being on -_-");
        return;
    }
    led_task_set_blink_period_ms(period_ms);
}

void help_callback();

void mem_callback(const char* args){
    
    uint32_t addr;
    if (sscanf(args, "%x", &addr) != 1) {
        printf("Error: invalid address format.\n");
        return;
    }
    mem_prot(addr);
}

void wmem_callback(const char* args){
        
    uint32_t addr, value;
    if (sscanf(args, "%x %x", &addr, &value) != 2) {
        printf("Error: invalid arguments.\n");
        return;
    }
    wmem_prot(addr, value);
}

void get_adc_callback(){
    float voltage_V = adc_measure();
    printf("%f\n", voltage_V);
}

void get_temp_callback(){
    float temp_C = temp_measure();
    printf("%f\n", temp_C);
}

api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
	{"off", led_off_callback, "turn led off"},
	{"on", led_on_callback, "turn led on"},
	{"blink", led_blink_callback, "get led to blink"},
	{"set_period", led_blink_set_period_ms_callback, "set blinking period"},
	{"help", help_callback, "prints command descriptions"},	
    {"mem", mem_callback, "read memory"},	
    {"wmem", wmem_callback, "write memory"},
    {"get_adc", get_adc_callback, "get current voltage"},
    {"get_temp", get_temp_callback, "get current temperature"},
	{NULL, NULL, NULL},
};

void help_callback(){
    api_t* api = device_api;   
    int commands_count = 0;
    if (api != 0){
        while (api[commands_count].command_name != 0){
            printf("command '%s': '%s' \n", api[commands_count].command_name, api[commands_count].command_help);
            commands_count++;
        }
    } 
}

char* comm;

int main()
{
    stdio_task_init();
    stdio_init_all();
    protocol_task_init(device_api);
    led_task_init();
    adc_task_init();
    
    while (1){
        comm = stdio_task_handle();
        led_task_handle();
        protocol_task_handle(comm);
    }
}

