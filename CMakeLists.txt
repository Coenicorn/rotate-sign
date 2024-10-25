cmake_minimum_required(VERSION 3.30)

set(picotool_DIR /home/coneicorn/dev/pico/picotool)
set(PICO_SDK_PATH /home/coenicorn/dev/pico/pico-sdk)

set(LIB_PATH ./lib)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rotate_sign)

enable_language( C CXX ASM ) 

pico_sdk_init()

add_subdirectory(${LIB_PATH}/pico-servo)

add_executable(main src/main.c)

target_include_directories(main PUBLIC ${LIB_PATH}/pico-servo/include)

target_link_libraries(main pico_stdlib hardware_i2c hardware_pwm)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

pico_add_extra_outputs(main)
