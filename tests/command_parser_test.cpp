#include "command_parser.hpp"
#include <gtest/gtest.h>
#include <string>
#include "command_id.hpp"
#include "lexer.hpp"
#include "parsed_command.hpp"

namespace fluffy_tribble {
namespace {

Pipe parse_line(const std::string &line) {
    Lexer lexer;
    TokenStream ts = lexer.tokenize(line);
    CommandParser parser;
    return parser.parse(ts);
}

TEST(CommandParserTest, EmptyLine) {
    Pipe pipe = parse_line("");
    EXPECT_TRUE(pipe.empty());
}

TEST(CommandParserTest, SingleCommandNoArgs) {
    Pipe pipe = parse_line("pwd");
    ASSERT_EQ(pipe.size(), 1u);
    EXPECT_EQ(pipe[0].name, "pwd");
    EXPECT_TRUE(pipe[0].args.empty());
    EXPECT_EQ(pipe[0].id, CommandID::BUILTIN);
}

TEST(CommandParserTest, CommandWithArgs) {
    Pipe pipe = parse_line("echo hello world");
    ASSERT_EQ(pipe.size(), 1u);
    EXPECT_EQ(pipe[0].name, "echo");
    ASSERT_EQ(pipe[0].args.size(), 2u);
    EXPECT_EQ(pipe[0].args[0], "hello");
    EXPECT_EQ(pipe[0].args[1], "world");
    EXPECT_EQ(pipe[0].id, CommandID::BUILTIN);
}

TEST(CommandParserTest, QuotedArg) {
    Pipe pipe = parse_line("echo 'hello world'");
    ASSERT_EQ(pipe.size(), 1u);
    EXPECT_EQ(pipe[0].name, "echo");
    ASSERT_EQ(pipe[0].args.size(), 1u);
    EXPECT_EQ(pipe[0].args[0], "hello world");
}

TEST(CommandParserTest, ExitCommand) {
    Pipe pipe = parse_line("exit");
    ASSERT_EQ(pipe.size(), 1u);
    EXPECT_EQ(pipe[0].name, "exit");
    EXPECT_EQ(pipe[0].id, CommandID::EXIT);
}

TEST(CommandParserTest, Assignment) {
    Pipe pipe = parse_line("VAR=value");
    ASSERT_EQ(pipe.size(), 1u);
    EXPECT_EQ(pipe[0].name, "VAR");
    ASSERT_EQ(pipe[0].args.size(), 1u);
    EXPECT_EQ(pipe[0].args[0], "value");
    EXPECT_EQ(pipe[0].id, CommandID::ASSIGN);
}

TEST(CommandParserTest, ExternalCommand) {
    Pipe pipe = parse_line("unknown_cmd_xyz arg");
    ASSERT_EQ(pipe.size(), 1u);
    EXPECT_EQ(pipe[0].name, "unknown_cmd_xyz");
    ASSERT_EQ(pipe[0].args.size(), 1u);
    EXPECT_EQ(pipe[0].args[0], "arg");
    EXPECT_EQ(pipe[0].id, CommandID::EXTERNAL);
}

}  // namespace
}  // namespace fluffy_tribble
