/** @file test_builtins.cpp
 *  Юнит-тесты для встроенных команд.
 */

#include "builtins.hpp"
#include <catch2/catch.hpp>
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace cli;

TEST_CASE("echo prints arguments", "[builtins][echo]") {
  std::ostringstream out, err;
  RunResult r = run_echo({"echo", "hello", "world"}, out, err);
  REQUIRE(r.exit_code == 0);
  REQUIRE(!r.exit_shell);
  REQUIRE(out.str() == "hello world\n");
  REQUIRE(err.str().empty());
}

TEST_CASE("echo single argument", "[builtins][echo]") {
  std::ostringstream out, err;
  RunResult r = run_echo({"echo", "x"}, out, err);
  REQUIRE(r.exit_code == 0);
  REQUIRE(out.str() == "x\n");
}

TEST_CASE("echo no arguments", "[builtins][echo]") {
  std::ostringstream out, err;
  RunResult r = run_echo({"echo"}, out, err);
  REQUIRE(r.exit_code == 0);
  REQUIRE(out.str().empty());
}

TEST_CASE("exit sets exit_shell", "[builtins][exit]") {
  std::ostringstream out, err;
  RunResult r = run_exit({"exit"}, out, err);
  REQUIRE(r.exit_code == 0);
  REQUIRE(r.exit_shell);
  REQUIRE(out.str().empty());
}

TEST_CASE("pwd returns current directory", "[builtins][pwd]") {
  std::ostringstream out, err;
  RunResult r = run_pwd({"pwd"}, out, err);
  REQUIRE(r.exit_code == 0);
  REQUIRE(!out.str().empty());
  REQUIRE(out.str().back() == '\n');
  REQUIRE(err.str().empty());
}

TEST_CASE("cat missing file", "[builtins][cat]") {
  std::ostringstream out, err;
  RunResult r = run_cat({"cat", "/nonexistent/file"}, out, err);
  REQUIRE(r.exit_code != 0);
  REQUIRE(!err.str().empty());
  REQUIRE(out.str().empty());
}

TEST_CASE("cat missing operand", "[builtins][cat]") {
  std::ostringstream out, err;
  RunResult r = run_cat({"cat"}, out, err);
  REQUIRE(r.exit_code != 0);
  REQUIRE(!err.str().empty());
}

TEST_CASE("wc missing file", "[builtins][wc]") {
  std::ostringstream out, err;
  RunResult r = run_wc({"wc", "/nonexistent"}, out, err);
  REQUIRE(r.exit_code != 0);
  REQUIRE(!err.str().empty());
}

TEST_CASE("wc missing operand", "[builtins][wc]") {
  std::ostringstream out, err;
  RunResult r = run_wc({"wc"}, out, err);
  REQUIRE(r.exit_code != 0);
  REQUIRE(!err.str().empty());
}

TEST_CASE("cat and wc with real file", "[builtins][cat][wc]") {
  // Create temp file
  const char* name = "test_builtins_temp.txt";
  {
    std::ofstream f(name);
    f << "one two three\nfour five\n";
  }
  std::ostringstream out, err;

  RunResult cr = run_cat({"cat", name}, out, err);
  REQUIRE(cr.exit_code == 0);
  REQUIRE(out.str() == "one two three\nfour five\n");
  out.str("");
  err.str("");

  RunResult wr = run_wc({"wc", name}, out, err);
  REQUIRE(wr.exit_code == 0);
  REQUIRE(err.str().empty());
  // 2 lines, 5 words, 22 bytes (depending on line endings)
  REQUIRE(out.str().find('2') != std::string::npos);
  REQUIRE(out.str().find('5') != std::string::npos);

  std::remove(name);
}
