# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

set(PROJECT_NAME hackusu2024)

project(${PROJECT_NAME} C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME}
        ${SRC_FILES}
)

# Add include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME} pico_stdlib)

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME}
        hardware_spi
)

pico_add_extra_outputs(${PROJECT_NAME})
