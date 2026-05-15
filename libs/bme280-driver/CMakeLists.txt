cmake_minimum_required(VERSION 3.15)

project(bme280-driver)

add_library(${PROJECT_NAME} STATIC
	"src/bme280-driver.c"
)

target_link_libraries(bme280-driver
    hardware_i2c
    hardware_gpio
    pico_stdlib
)


target_include_directories(${PROJECT_NAME} PUBLIC
	"include"
)

