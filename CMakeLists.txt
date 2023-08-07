cmake_minimum_required(VERSION 3.26)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(caldo_node C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

pico_sdk_init()

add_executable(caldo_node main.cpp
        hx711_t.cpp
        hx711_t.h)

pico_add_extra_outputs(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE  extern/pico-scale/include)
#Link to pico stdlib
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
)

#Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)