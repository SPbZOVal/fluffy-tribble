#include "builtins.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include "execution_context.hpp"

namespace fluffy_tribble {
namespace {

TEST(BuiltinsTest, EchoEmpty) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    run_echo({}, in, out, err, ctx);
    EXPECT_EQ(out.str(), "\n");
}

TEST(BuiltinsTest, EchoOneArg) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    run_echo({"hello"}, in, out, err, ctx);
    EXPECT_EQ(out.str(), "hello\n");
}

TEST(BuiltinsTest, EchoMultipleArgs) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    run_echo({"a", "b", "c"}, in, out, err, ctx);
    EXPECT_EQ(out.str(), "a b c\n");
}

TEST(BuiltinsTest, Pwd) {
    ExecutionContext ctx;
    ctx.set_cwd("/tmp");
    std::istringstream in;
    std::ostringstream out, err;
    run_pwd({}, in, out, err, ctx);
    EXPECT_EQ(out.str(), "/tmp\n");
}

TEST(BuiltinsTest, ExitSetsFlag) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    run_exit({}, in, out, err, ctx);
    EXPECT_TRUE(ctx.is_exit());
    EXPECT_EQ(ctx.exit_code(), 0);
}

TEST(BuiltinsTest, ExitWithCode) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    run_exit({"5"}, in, out, err, ctx);
    EXPECT_TRUE(ctx.is_exit());
    EXPECT_EQ(ctx.exit_code(), 5);
}

TEST(BuiltinsTest, CatStdin) {
    ExecutionContext ctx;
    std::istringstream in("line1\nline2\n");
    std::ostringstream out, err;
    run_cat({}, in, out, err, ctx);
    EXPECT_EQ(out.str(), "line1\nline2\n");
}

TEST(BuiltinsTest, WcStdin) {
    ExecutionContext ctx;
    std::istringstream in("one two\nthree\n");
    std::ostringstream out, err;
    run_wc({}, in, out, err, ctx);
    EXPECT_EQ(out.str(), "2 3 14\n");
}

}  // namespace
}  // namespace fluffy_tribble
