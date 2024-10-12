# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

set(PICO_BOARD pico2 CACHE STRING "Board type")
# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico-sdk/pico_sdk_init.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(SandBox C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# COMMON
set(COMMON_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/include)
set(COMMON_SOURCES src/Renderer.c src/OBJReader.c)

# ---SIM TARGET---
find_package(SDL2 REQUIRED)

add_executable(SandBoxSim ${COMMON_SOURCES} src/SandBoxSim.c)
target_include_directories(SandBoxSim PRIVATE ${SDL2_INCLUDE_DIRS} ${COMMON_INCLUDES})
target_link_libraries(SandBoxSim ${SDL2_LIBRARIES})

# ---PICO TARGET--- 
add_executable(SandBoxPico ${COMMON_SOURCES} src/SandBoxPico.c )

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(SandBoxPico 0)
pico_enable_stdio_usb(SandBoxPico 1)

# Add the standard library to the build
target_link_libraries(SandBoxPico
        pico_stdlib
        FreeRTOS-Kernel)

# Add the standard include files to the build
target_include_directories(SandBoxPico PRIVATE ${COMMON_INCLUDES})

# Add any user requested libraries
target_link_libraries(SandBoxPico 
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_interp
        hardware_timer
        hardware_watchdog
        hardware_clocks
        
        )

pico_add_extra_outputs(SandBoxPico)