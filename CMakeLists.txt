cmake_minimum_required(VERSION 3.10)
project(AcademicProgressTracker VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add option to enable or disable testing
option(BUILD_TESTING "Build the testing tree" ON)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Define include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Get all source files (excluding main.cpp)
file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Create a library with all sources except main.cpp
add_library(academic_core ${LIB_SOURCES})
target_include_directories(academic_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Get main.cpp separately
file(GLOB MAIN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Create the main executable
add_executable(academic_tracker ${MAIN_SOURCE})
target_link_libraries(academic_tracker PRIVATE academic_core)

# Enable warnings
if(MSVC)
    target_compile_options(academic_tracker PRIVATE /W4)
    target_compile_options(academic_core PRIVATE /W4)
else()
    target_compile_options(academic_tracker PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(academic_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS academic_tracker DESTINATION bin)

# Testing configuration
if(BUILD_TESTING)
    enable_testing()

    # Create test executable
    file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
    
    # Add test executable
    add_executable(unit_tests ${TEST_SOURCES})
    
    # Link with Google Test and our library
    target_link_libraries(unit_tests PRIVATE
        academic_core
        gtest
        gtest_main
        pthread
    )
    
    # Add tests to CTest
    add_test(NAME UnitTests COMMAND unit_tests)
    
    # Add a custom target for running the tests
    add_custom_target(run_tests ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Running all tests..."
        COMMAND ${CMAKE_BINARY_DIR}/unit_tests
        DEPENDS unit_tests
        COMMENT "Running unit tests"
    )
endif()