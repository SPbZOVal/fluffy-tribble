cmake_minimum_required(VERSION 3.14)
project(fluffy_tribble LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# Enable generation of compile_commands.json for better IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Library with all logic (shared by main and tests)
add_library(fluffy_tribble_lib
  src/lexer.cpp
  src/execution_context.cpp
  src/command_parser.cpp
  src/command_manager.cpp
  src/builtins.cpp
  src/external_runner.cpp
  src/command_executor.cpp
)
target_include_directories(fluffy_tribble_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Main executable
add_executable(fluffy_tribble src/main.cpp)
target_link_libraries(fluffy_tribble PRIVATE fluffy_tribble_lib)


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

add_executable(fluffy_tribble_test
  tests/lexer_test.cpp
  tests/command_parser_test.cpp
  tests/command_manager_test.cpp
  tests/builtins_test.cpp
  tests/execution_context_test.cpp
)
target_link_libraries(fluffy_tribble_test PRIVATE fluffy_tribble_lib GTest::gtest GTest::gtest_main)
add_test(NAME fluffy_tribble_test COMMAND fluffy_tribble_test)
