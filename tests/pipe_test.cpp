#include <gtest/gtest.h>
#include <sstream>
#include "command_parser.hpp"
#include "execution_context.hpp"
#include "lexer.hpp"
#include "pipe_executor.hpp"

namespace fluffy_tribble {
namespace {

TEST(PipeTest, BasicPipelineExecution) {
    ExecutionContext ctx;
    Lexer lexer;
    CommandParser parser;
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;

    auto tokens1 = lexer.tokenize("echo hello | cat", ctx);
    auto pipe1 = parser.parse(tokens1);
    ASSERT_EQ(pipe1.size(), 2U);
    PipeExecutor::execute(pipe1, in, out, err, ctx);
    EXPECT_EQ(out.str(), "hello\n");
    out.str("");
    out.clear();

    auto tokens2 = lexer.tokenize("echo 'line one' | cat | cat", ctx);
    auto pipe2 = parser.parse(tokens2);
    ASSERT_EQ(pipe2.size(), 3U);
    PipeExecutor::execute(pipe2, in, out, err, ctx);
    EXPECT_EQ(out.str(), "line one\n");
    out.str("");
    out.clear();

    auto tokens3 = lexer.tokenize("echo 'one two three' | wc", ctx);
    auto pipe3 = parser.parse(tokens3);
    PipeExecutor::execute(pipe3, in, out, err, ctx);
    EXPECT_EQ(out.str(), "1 3 14\n");
}

TEST(PipeTest, VariableExpansionInPipes) {
    ExecutionContext ctx;
    ctx.set_env("GREETING", "Hello");
    ctx.set_env("NAME", "World");
    ctx.set_env("CMD", "ec");
    ctx.set_env("HO", "ho");

    Lexer lexer;
    CommandParser parser;
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;

    auto tokens1 = lexer.tokenize("echo $GREETING", ctx);
    auto pipe1 = parser.parse(tokens1);
    PipeExecutor::execute(pipe1, in, out, err, ctx);
    EXPECT_EQ(out.str(), "Hello\n");
    out.str("");
    out.clear();

    auto tokens2 = lexer.tokenize("$CMD$HO hello", ctx);
    auto pipe2 = parser.parse(tokens2);
    ASSERT_EQ(pipe2[0].name, "echo");
    PipeExecutor::execute(pipe2, in, out, err, ctx);
    EXPECT_EQ(out.str(), "hello\n");
    out.str("");
    out.clear();

    ctx.set_env("A", "hel");
    ctx.set_env("B", "lo");
    ctx.set_env("C", " wor");
    ctx.set_env("D", "ld");
    auto tokens3 = lexer.tokenize("echo $A$B$C$D", ctx);
    auto pipe3 = parser.parse(tokens3);
    PipeExecutor::execute(pipe3, in, out, err, ctx);
    EXPECT_EQ(out.str(), "hello world\n");
}

TEST(PipeTest, QuotingAndEscapingBehavior) {
    ExecutionContext ctx;
    ctx.set_env("VAR", "test");

    Lexer lexer;
    CommandParser parser;
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;

    auto tokens1 = lexer.tokenize("echo '$VAR'", ctx);
    auto pipe1 = parser.parse(tokens1);
    PipeExecutor::execute(pipe1, in, out, err, ctx);
    EXPECT_EQ(out.str(), "$VAR\n");
    out.str("");
    out.clear();

    auto tokens2 = lexer.tokenize("echo \"$VAR\"", ctx);
    auto pipe2 = parser.parse(tokens2);
    PipeExecutor::execute(pipe2, in, out, err, ctx);
    EXPECT_EQ(out.str(), "test\n");
    out.str("");
    out.clear();

    auto tokens3 = lexer.tokenize(R"(echo "\$VAR")", ctx);
    auto pipe3 = parser.parse(tokens3);
    PipeExecutor::execute(pipe3, in, out, err, ctx);
    EXPECT_EQ(out.str(), "$VAR\n");
    out.str("");
    out.clear();

    auto tokens4 = lexer.tokenize(R"(echo "Say \"hello\"")", ctx);
    auto pipe4 = parser.parse(tokens4);
    PipeExecutor::execute(pipe4, in, out, err, ctx);
    EXPECT_EQ(out.str(), "Say \"hello\"\n");
    out.str("");
    out.clear();

    auto tokens5 = lexer.tokenize("echo 'test\\nvalue'", ctx);
    auto pipe5 = parser.parse(tokens5);
    PipeExecutor::execute(pipe5, in, out, err, ctx);
    EXPECT_EQ(out.str(), "test\\nvalue\n");
    out.str("");
    out.clear();

    auto tokens6 = lexer.tokenize("echo one\\ two\\ three", ctx);
    auto pipe6 = parser.parse(tokens6);
    PipeExecutor::execute(pipe6, in, out, err, ctx);
    EXPECT_EQ(out.str(), "one two three\n");
    out.str("");
    out.clear();

    auto tokens7 = lexer.tokenize("echo '$VAR' \"$VAR\"", ctx);
    auto pipe7 = parser.parse(tokens7);
    PipeExecutor::execute(pipe7, in, out, err, ctx);
    EXPECT_EQ(out.str(), "$VAR test\n");
}

TEST(PipeTest, AssignmentAndExit) {
    ExecutionContext ctx;
    ctx.set_env("x", "ex");
    ctx.set_env("y", "it");

    Lexer lexer;
    CommandParser parser;
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;

    auto tokens = lexer.tokenize("$x$y", ctx);
    auto pipe = parser.parse(tokens);

    ASSERT_EQ(pipe.size(), 1U);
    EXPECT_EQ(pipe[0].name, "exit");
    EXPECT_EQ(pipe[0].id, CommandID::EXIT);

    PipeExecutor::execute(pipe, in, out, err, ctx);
    EXPECT_TRUE(ctx.is_exit());
}

TEST(PipeTest, ErrorUnclosedQuoteInPipe) {
    ExecutionContext ctx;
    Lexer lexer;

    EXPECT_THROW(
        lexer.tokenize("echo 'hello | cat", ctx),
        std::runtime_error
    );
}

TEST(PipeTest, EmptyPipe) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;

    Pipe empty_pipe;
    PipeExecutor::execute(empty_pipe, in, out, err, ctx);
    EXPECT_EQ(out.str(), "");
}

TEST(PipeTest, PipeWithMultipleExternalCommands) {
    ExecutionContext ctx;
    Lexer lexer;
    CommandParser parser;
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;

    auto tokens = lexer.tokenize("echo 'test line' | cat | wc", ctx);
    auto pipe = parser.parse(tokens);
    ASSERT_EQ(pipe.size(), 3U);
    PipeExecutor::execute(pipe, in, out, err, ctx);
    EXPECT_EQ(out.str(), "1 2 10\n");
}

TEST(PipeTest, AssignmentInPipe) {
    ExecutionContext ctx;
    ctx.set_env("VAR", "test");
    Lexer lexer;
    CommandParser parser;
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;

    auto tokens = lexer.tokenize("$VAR=newvalue", ctx);
    auto pipe = parser.parse(tokens);
    ASSERT_EQ(pipe.size(), 1U);
    EXPECT_EQ(pipe[0].id, CommandID::ASSIGN);
    PipeExecutor::execute(pipe, in, out, err, ctx);
    EXPECT_EQ(ctx.env().at("VAR"), "newvalue");
}

}  // namespace
}  // namespace fluffy_tribble