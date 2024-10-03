# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(SandBox C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(SandBox src/SandBox.c )

pico_set_program_name(SandBox "SandBox")
pico_set_program_version(SandBox "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(SandBox 0)
pico_enable_stdio_usb(SandBox 1)

# Add the standard library to the build
target_link_libraries(SandBox
        pico_stdlib
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap1)

# Add the standard include files to the build
target_include_directories(SandBox PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(SandBox 
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_interp
        hardware_timer
        hardware_watchdog
        hardware_clocks
        
        )

pico_add_extra_outputs(SandBox)