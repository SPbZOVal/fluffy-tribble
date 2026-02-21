cmake_minimum_required(VERSION 3.14)
project(fluffy_tribble LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# Enable generation of compile_commands.json for better IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Fetch argparse library
include(FetchContent)
FetchContent_Declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
  GIT_TAG v3.2
)
FetchContent_MakeAvailable(argparse)

# Set properties
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

set(PROJECT_LIB_NAME ${PROJECT_NAME}_lib)
set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/main.cpp)

file(GLOB_RECURSE PROJECT_TEST_SOURCES ${PROJECT_TEST_DIR}/*.cpp) 

# Library with all logic (shared by main and tests)
add_library(${PROJECT_LIB_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_LIB_NAME} PUBLIC 
  ${CMAKE_SOURCE_DIR}/include
  ${argparse_SOURCE_DIR}/include
)

# Main executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIB_NAME})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_LIB_NAME} PUBLIC Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Tests (GTest)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(${PROJECT_TEST_NAME} ${PROJECT_TEST_SOURCES})
target_link_libraries(${PROJECT_TEST_NAME} PRIVATE ${PROJECT_LIB_NAME} GTest::gtest GTest::gtest_main)
target_include_directories(${PROJECT_TEST_NAME} PRIVATE ${argparse_SOURCE_DIR}/include)
add_test(NAME ${PROJECT_TEST_NAME} COMMAND ${PROJECT_TEST_NAME})
