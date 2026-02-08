/** @file test_tokenizer.cpp
 *  Юнит-тесты для токенайзера.
 */

#include "tokenizer.hpp"
#include <catch2/catch.hpp>

using namespace cli;

TEST_CASE("Empty line yields empty list", "[tokenizer]") {
  REQUIRE(tokenize("").empty());
  REQUIRE(tokenize("   ").empty());
  REQUIRE(tokenize("\t\n  ").empty());
}

TEST_CASE("Single word", "[tokenizer]") {
  auto t = tokenize("echo");
  REQUIRE(t.size() == 1);
  REQUIRE(t[0] == "echo");
}

TEST_CASE("Several words", "[tokenizer]") {
  auto t = tokenize("echo hello world");
  REQUIRE(t.size() == 3);
  REQUIRE(t[0] == "echo");
  REQUIRE(t[1] == "hello");
  REQUIRE(t[2] == "world");
}

TEST_CASE("Double quotes make one argument", "[tokenizer]") {
  auto t = tokenize("echo \"hello world\"");
  REQUIRE(t.size() == 2);
  REQUIRE(t[0] == "echo");
  REQUIRE(t[1] == "hello world");
}

TEST_CASE("Single quotes make one argument", "[tokenizer]") {
  auto t = tokenize("echo 'hello world'");
  REQUIRE(t.size() == 2);
  REQUIRE(t[0] == "echo");
  REQUIRE(t[1] == "hello world");
}

TEST_CASE("Mixed quoted and unquoted", "[tokenizer]") {
  auto t = tokenize("cat \"file name.txt\"");
  REQUIRE(t.size() == 2);
  REQUIRE(t[0] == "cat");
  REQUIRE(t[1] == "file name.txt");
}

TEST_CASE("Quoted empty string", "[tokenizer]") {
  auto t = tokenize("echo \"\"");
  REQUIRE(t.size() == 2);
  REQUIRE(t[0] == "echo");
  REQUIRE(t[1] == "");
}

TEST_CASE("Multiple quoted segments", "[tokenizer]") {
  auto t = tokenize("a \"b c\" d 'e f' g");
  REQUIRE(t.size() == 5);
  REQUIRE(t[0] == "a");
  REQUIRE(t[1] == "b c");
  REQUIRE(t[2] == "d");
  REQUIRE(t[3] == "e f");
  REQUIRE(t[4] == "g");
}

TEST_CASE("Leading and trailing spaces", "[tokenizer]") {
  auto t = tokenize("  echo foo  ");
  REQUIRE(t.size() == 2);
  REQUIRE(t[0] == "echo");
  REQUIRE(t[1] == "foo");
}
