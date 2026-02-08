/** @file test_builtins.cpp
 *  Юнит-тесты для встроенных команд.
 */

#include "builtins.hpp"
#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace cli;

TEST(BuiltinsEchoTest, PrintsArguments) {
  std::ostringstream out, err;
  RunResult r = run_echo({"echo", "hello", "world"}, out, err);
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.exit_shell);
  EXPECT_EQ(out.str(), "hello world\n");
  EXPECT_TRUE(err.str().empty());
}

TEST(BuiltinsEchoTest, SingleArgument) {
  std::ostringstream out, err;
  RunResult r = run_echo({"echo", "x"}, out, err);
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(out.str(), "x\n");
}

TEST(BuiltinsEchoTest, NoArguments) {
  std::ostringstream out, err;
  RunResult r = run_echo({"echo"}, out, err);
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(out.str().empty());
}

TEST(BuiltinsExitTest, SetsExitShell) {
  std::ostringstream out, err;
  RunResult r = run_exit({"exit"}, out, err);
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.exit_shell);
  EXPECT_TRUE(out.str().empty());
}

TEST(BuiltinsPwdTest, ReturnsCurrentDirectory) {
  std::ostringstream out, err;
  RunResult r = run_pwd({"pwd"}, out, err);
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(out.str().empty());
  EXPECT_EQ(out.str().back(), '\n');
  EXPECT_TRUE(err.str().empty());
}

TEST(BuiltinsCatTest, MissingFile) {
  std::ostringstream out, err;
  RunResult r = run_cat({"cat", "/nonexistent/file"}, out, err);
  EXPECT_NE(r.exit_code, 0);
  EXPECT_FALSE(err.str().empty());
  EXPECT_TRUE(out.str().empty());
}

TEST(BuiltinsCatTest, MissingOperand) {
  std::ostringstream out, err;
  RunResult r = run_cat({"cat"}, out, err);
  EXPECT_NE(r.exit_code, 0);
  EXPECT_FALSE(err.str().empty());
}

TEST(BuiltinsWcTest, MissingFile) {
  std::ostringstream out, err;
  RunResult r = run_wc({"wc", "/nonexistent"}, out, err);
  EXPECT_NE(r.exit_code, 0);
  EXPECT_FALSE(err.str().empty());
}

TEST(BuiltinsWcTest, MissingOperand) {
  std::ostringstream out, err;
  RunResult r = run_wc({"wc"}, out, err);
  EXPECT_NE(r.exit_code, 0);
  EXPECT_FALSE(err.str().empty());
}

TEST(BuiltinsCatWcTest, CatAndWcWithRealFile) {
  const char* name = "test_builtins_temp.txt";
  {
    std::ofstream f(name);
    f << "one two three\nfour five\n";
  }
  std::ostringstream out, err;

  RunResult cr = run_cat({"cat", name}, out, err);
  EXPECT_EQ(cr.exit_code, 0);
  EXPECT_EQ(out.str(), "one two three\nfour five\n");
  out.str("");
  err.str("");

  RunResult wr = run_wc({"wc", name}, out, err);
  EXPECT_EQ(wr.exit_code, 0);
  EXPECT_TRUE(err.str().empty());
  EXPECT_NE(out.str().find('2'), std::string::npos);
  EXPECT_NE(out.str().find('5'), std::string::npos);

  std::remove(name);
}
