
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(UART_Receiver_Printer VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the Boost libraries
find_package(Boost REQUIRED COMPONENTS system thread json)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost libraries")
endif()

# Add executable
add_executable(uart_receiver_printer uart_receiver_printer.cpp)

# Link Boost libraries
target_link_libraries(uart_receiver_printer PRIVATE Boost::system Boost::thread Boost::json)

# Include Boost headers
target_include_directories(uart_receiver_printer PRIVATE ${Boost_INCLUDE_DIRS})
