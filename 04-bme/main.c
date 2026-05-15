#include "stdio.h" // стандартная библиотека ввода-вывода
#include "stdlib.h" // стандартная библиотека..?
#include "pico/stdlib.h" // базовое взаимодействие с микроконтроллером
#include "stdio-task/stdio-task.h"
#include "protocol-task.h"
#include "led-task/led-task.h"
#include "hardware/i2c.h"
#include "bme280-driver.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"


void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}

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

void read_reg_callback(const char* args){

    uint32_t addr, N;
    if (sscanf(args, "%x %x", &addr, &N) != 2) {
        printf("Error: invalid arguments.\n");
        return;
    }
    if (addr > 0xFF || N == 0 || N > 0xFF || addr + N > 0x100) {
        printf("Error: addr (0x%X) and count (0x%X) must satisfy:\n", addr, N);
        printf("  addr ≤ 0xFF, count between 1 and 0xFF, addr+count ≤ 0x100\n");
        return;
    }

    uint8_t buffer[256] = {0};
    bme280_read_regs((uint8_t)addr, buffer, (uint8_t)N);

    for (int i = 0; i < (int)N; i++) {
        printf("bme280 register [0x%X] = 0x%X\n", addr + i, buffer[i]);
    }
}

void write_reg_callback(const char* args){

    uint32_t addr, value;
    if (sscanf(args, "%x %x", &addr, &value) != 2) {
        printf("Error: invalid arguments.\n");
        return;
    }
    if (addr > 0xFF || value > 0xFF ) {
        printf("Error: addr (0x%X) and value (0x%X) must satisfy:\n", addr, value);
        printf("  addr ≤ 0xFF, value ≤ 0xFF\n");
        return;
    }

    bme280_write_reg((uint8_t)addr, (uint8_t)value);
}

void temp_raw_callback(const char* args){
    uint16_t counts = bme280_read_temp_raw();
    printf("temperature counts %d \n", counts);
}

void press_raw_callback(const char* args){
    uint16_t counts = bme280_read_press_raw();
    printf("pressure counts %d \n", counts);
}

void hum_raw_callback(const char* args){
    uint16_t counts = bme280_read_hum_raw();
    printf("humidity counts %d \n", counts);
}

void temp_callback(const char* args){
    float temp = bme280_read_temp();
    printf("%f\n", temp);
}

void press_callback(const char* args){
    float press = bme280_read_press();
    printf("%f\n", press);
}

void hum_callback(const char* args){
    float hum = bme280_read_hum();
    printf("%f\n", hum);
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
    {"read_regs", read_reg_callback, "read registers"},
    {"write_regs", write_reg_callback, "write registers"},
    {"temp_raw", temp_raw_callback, "get raw temperature"},
    {"press_raw", press_raw_callback, "get raw pressure"},
    {"hum_raw", hum_raw_callback, "get raw humidity"},
    {"temp", temp_callback, "get temperature"},
    {"press", press_callback, "get pressure"},
    {"hum", hum_callback, "get humidity"},

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
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    bme280_init(rp2040_i2c_read, rp2040_i2c_write);

    while (1){
        comm = stdio_task_handle();
        led_task_handle();
        protocol_task_handle(comm);
    }
}

