cmake_minimum_required(VERSION 3.14)
project(lka LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Library with all logic (shared by main and tests)
add_library(lka_lib
  src/lexer.cpp
  src/execution_context.cpp
  src/command_parser.cpp
  src/command_manager.cpp
  src/builtins.cpp
  src/external_runner.cpp
  src/command_executor.cpp
  src/pipe_executor.cpp
)
target_include_directories(lka_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Main executable
add_executable(lka src/main.cpp)
target_link_libraries(lka PRIVATE lka_lib)


# Tests (GTest)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(lka_test
  tests/lexer_test.cpp
  tests/command_parser_test.cpp
  tests/command_manager_test.cpp
  tests/builtins_test.cpp
  tests/execution_context_test.cpp
)
target_link_libraries(lka_test PRIVATE lka_lib GTest::gtest GTest::gtest_main)
add_test(NAME lka_test COMMAND lka_test)
