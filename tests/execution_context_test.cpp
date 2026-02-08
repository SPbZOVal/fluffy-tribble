#include "execution_context.hpp"
#include <gtest/gtest.h>

namespace lka {
namespace {

TEST(ExecutionContextTest, Env) {
    ExecutionContext ctx;
    EXPECT_TRUE(ctx.env().empty() || !ctx.env().empty());
    ctx.set_env("TEST_VAR", "test_value");
    auto it = ctx.env().find("TEST_VAR");
    ASSERT_NE(it, ctx.env().end());
    EXPECT_EQ(it->second, "test_value");
}

TEST(ExecutionContextTest, Cwd) {
    ExecutionContext ctx;
    std::string cwd = ctx.cwd();
    EXPECT_FALSE(cwd.empty());
    ctx.set_cwd("/tmp");
    EXPECT_EQ(ctx.cwd(), "/tmp");
}

TEST(ExecutionContextTest, ExitFlag) {
    ExecutionContext ctx;
    EXPECT_FALSE(ctx.is_exit());
    ctx.set_exit(true);
    EXPECT_TRUE(ctx.is_exit());
}

TEST(ExecutionContextTest, LastStatus) {
    ExecutionContext ctx;
    EXPECT_EQ(ctx.last_status(), 0);
    ctx.set_last_status(42);
    EXPECT_EQ(ctx.last_status(), 42);
}

TEST(ExecutionContextTest, ExitCode) {
    ExecutionContext ctx;
    EXPECT_EQ(ctx.exit_code(), 0);
    ctx.set_exit_code(1);
    EXPECT_EQ(ctx.exit_code(), 1);
}

}  // namespace
}  // namespace lka
