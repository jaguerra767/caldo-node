cmake_minimum_required(VERSION 3.24)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(caldo_node C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

pico_sdk_init()

add_executable(caldo_node
        main.c
)

pico_add_extra_outputs(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
        extern/pico-scale/include
        extern/pico-scale/extern/hx711-pico-c/include)
#Link to pico stdlib
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_pio
        hx711-pico-c
)
add_subdirectory(/Users/jguerra/caldo/nodes/caldo-node/extern/pico-scale/extern/hx711-pico-c)
#Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)