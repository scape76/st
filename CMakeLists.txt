# cmake_minimum_required(VERSION 3.10)
# project(AcademicProgressTracker VERSION 1.0)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# option(BUILD_TESTING "Build the testing tree" ON)

# include(FetchContent)

# FetchContent_Declare(
#     googletest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG release-1.12.1
# )
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# list(FILTER LIB_SOURCES EXCLUDE REGEX ".*(main|bindings)\\.cpp$")

# add_library(academic_core ${LIB_SOURCES})
# target_include_directories(academic_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# file(GLOB MAIN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# add_executable(academic_tracker ${MAIN_SOURCE})
# target_link_libraries(academic_tracker PRIVATE academic_core)

# if(MSVC)
#     target_compile_options(academic_tracker PRIVATE /W4)
#     target_compile_options(academic_core PRIVATE /W4)
# else()
#     target_compile_options(academic_tracker PRIVATE -Wall -Wextra -Wpedantic)
#     target_compile_options(academic_core PRIVATE -Wall -Wextra -Wpedantic)
# endif()

# install(TARGETS academic_tracker DESTINATION bin)

# if(BUILD_TESTING)
#     enable_testing()

#     file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

#     add_executable(unit_tests ${TEST_SOURCES})

#     target_link_libraries(unit_tests PRIVATE
#         academic_core
#         gtest
#         gtest_main
#         pthread
#     )

#     add_test(NAME UnitTests COMMAND unit_tests)

#     add_custom_target(run_tests ALL
#         COMMAND ${CMAKE_COMMAND} -E echo "Running all tests..."
#         COMMAND ${CMAKE_BINARY_DIR}/unit_tests
#         DEPENDS unit_tests
#         COMMENT "Running unit tests"
#     )
# endif()

# EMSCRIPTEN
cmake_minimum_required(VERSION 3.10)
project(AcademicProgressTrackerWASM VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS OFF)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED EMSCRIPTEN)
    message(FATAL_ERROR "This CMake file is intended to be used with Emscripten. Please use emcmake.")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*bindings\\.cpp$")

add_library(academic_core ${LIB_SOURCES})
target_include_directories(academic_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(BINDING_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/bindings.cpp")

add_executable(academic_wasm ${BINDING_SOURCE})
target_link_libraries(academic_wasm PRIVATE academic_core)

set_target_properties(academic_wasm PROPERTIES
  LINK_FLAGS "-s WASM=1 -s EXPORT_NAME='AcademicTracker' -s MODULARIZE=1 -s EXPORT_ES6=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s ASSERTIONS=1 --bind"
  OUTPUT_NAME "academic_tracker"
)

add_custom_command(TARGET academic_wasm POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/academic_tracker.js
    ${CMAKE_CURRENT_BINARY_DIR}/academic_tracker.wasm
    ${CMAKE_CURRENT_SOURCE_DIR}/client/src
  COMMENT "Copying WebAssembly files to web directory"
)

install(TARGETS academic_wasm DESTINATION web)

install(TARGETS academic_wasm DESTINATION web)
install(TARGETS academic_wasm DESTINATION web)

install(TARGETS academic_wasm DESTINATION web)
