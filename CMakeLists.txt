cmake_minimum_required(VERSION 3.14)
project(fluffy-tribble VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Static libraries for use in main and tests
add_library(cli_tokenizer STATIC src/tokenizer.cpp)
target_include_directories(cli_tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(cli_builtins STATIC src/builtins.cpp)
target_include_directories(cli_builtins PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(cli_external STATIC src/external_runner.cpp)
target_include_directories(cli_external PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(cli_executor STATIC src/executor.cpp)
target_include_directories(cli_executor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cli_executor PUBLIC cli_builtins cli_external)

# Main executable
add_executable(cli src/main.cpp)
target_link_libraries(cli PRIVATE cli_executor cli_tokenizer)
target_include_directories(cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Unit tests (Catch2)
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.10
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests
  tests/test_tokenizer.cpp
  tests/test_builtins.cpp
)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain cli_tokenizer cli_builtins)

include(CTest)
enable_testing()
add_test(NAME UnitTests COMMAND tests)
