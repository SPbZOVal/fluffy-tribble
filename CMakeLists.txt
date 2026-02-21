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
  GIT_TAG v3.0  # or use master for latest
)
FetchContent_MakeAvailable(argparse)

# Library with all logic (shared by main and tests)
add_library(fluffy_tribble_lib
  src/lexer.cpp
  src/execution_context.cpp
  src/command_parser.cpp
  src/command_manager.cpp
  src/builtins.cpp
  src/external_runner.cpp
  src/command_executor.cpp
  src/pipe_executor.cpp
)
target_include_directories(fluffy_tribble_lib PUBLIC 
  ${CMAKE_SOURCE_DIR}/include
  ${argparse_SOURCE_DIR}/include  # Add argparse include path
)

# Main executable
add_executable(fluffy_tribble src/main.cpp)
target_link_libraries(fluffy_tribble PRIVATE fluffy_tribble_lib)

find_package(Threads REQUIRED)
target_link_libraries(fluffy_tribble_lib PUBLIC Threads::Threads)
target_link_libraries(fluffy_tribble PRIVATE Threads::Threads)

# Tests (GTest)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(fluffy_tribble_test
  tests/lexer_test.cpp
  tests/command_parser_test.cpp
  tests/command_manager_test.cpp
  tests/builtins_test.cpp
  tests/execution_context_test.cpp
  tests/pipe_test.cpp
)
target_link_libraries(fluffy_tribble_test PRIVATE fluffy_tribble_lib GTest::gtest GTest::gtest_main)
target_include_directories(fluffy_tribble_test PRIVATE ${argparse_SOURCE_DIR}/include)
add_test(NAME fluffy_tribble_test COMMAND fluffy_tribble_test)
