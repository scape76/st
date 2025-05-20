cmake_minimum_required(VERSION 3.10)
project(AcademicProgressTracker VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Get all source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Create executable
add_executable(academic_tracker ${SOURCES})

# Enable warnings
if(MSVC)
    target_compile_options(academic_tracker PRIVATE /W4)
else()
    target_compile_options(academic_tracker PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS academic_tracker DESTINATION bin)