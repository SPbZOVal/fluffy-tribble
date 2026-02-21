#include "lexer.hpp"
#include <gtest/gtest.h>
#include "execution_context.hpp"
#include "token.hpp"

namespace fluffy_tribble {
namespace {

TEST(LexerTest, BasicTokenization) {
    ExecutionContext ctx;
    Lexer lexer;

    auto ts1 = lexer.tokenize("", ctx);
    ASSERT_EQ(ts1.size(), 1U);
    EXPECT_EQ(ts1[0].type, TokenType::EOF_);

    auto ts2 = lexer.tokenize("hello", ctx);
    ASSERT_GE(ts2.size(), 2U);
    EXPECT_EQ(ts2[0].value, "hello");

    auto ts3 = lexer.tokenize("echo hello world", ctx);
    ASSERT_GE(ts3.size(), 4U);
    EXPECT_EQ(ts3[0].value, "echo");
    EXPECT_EQ(ts3[1].value, "hello");
    EXPECT_EQ(ts3[2].value, "world");
}

TEST(LexerTest, QuotingBehavior) {
    ExecutionContext ctx;
    ctx.set_env("VAR", "value");
    Lexer lexer;

    auto ts1 = lexer.tokenize("'$VAR and text'", ctx);
    EXPECT_EQ(ts1[0].value, "$VAR and text");

    auto ts2 = lexer.tokenize("\"Hello, $VAR!\"", ctx);
    EXPECT_EQ(ts2[0].value, "Hello, value!");

    auto ts3 = lexer.tokenize("'$VAR' \"$VAR\"", ctx);
    EXPECT_EQ(ts3[0].value, "$VAR");
    EXPECT_EQ(ts3[1].value, "value");
}

TEST(LexerTest, VariableExpansionAndConcatenation) {
    ExecutionContext ctx;
    ctx.set_env("A", "hel");
    ctx.set_env("B", "lo");
    ctx.set_env("USER", "john");
    Lexer lexer;

    auto ts1 = lexer.tokenize("echo $USER", ctx);
    EXPECT_EQ(ts1[1].value, "john");

    auto ts2 = lexer.tokenize("$A$B", ctx);
    EXPECT_EQ(ts2[0].value, "hello");

    auto ts3 = lexer.tokenize("$A.txt", ctx);
    EXPECT_EQ(ts3[0].value, "hel.txt");

    auto ts4 = lexer.tokenize("$UNDEFINED", ctx);
    EXPECT_EQ(ts4.size(), 1U);
}

TEST(LexerTest, EscapeSequences) {
    ExecutionContext ctx;
    ctx.set_env("VAR", "value");
    Lexer lexer;

    auto ts1 = lexer.tokenize(R"('\n\t\$VAR')", ctx);
    EXPECT_EQ(ts1[0].value, "\\n\\t\\$VAR");

    auto ts2 = lexer.tokenize(R"("\$VAR")", ctx);
    EXPECT_EQ(ts2[0].value, "$VAR");

    auto ts3 = lexer.tokenize(R"("Say \"hi\"")", ctx);
    EXPECT_EQ(ts3[0].value, "Say \"hi\"");

    auto ts4 = lexer.tokenize(R"("line1\nline2")", ctx);
    EXPECT_EQ(ts4[0].value, "line1\nline2");

    auto ts5 = lexer.tokenize(R"("\a\b")", ctx);
    EXPECT_EQ(ts5[0].value, "\\a\\b");

    auto ts6 = lexer.tokenize("one\\ two", ctx);
    EXPECT_EQ(ts6[0].value, "one two");

    auto ts7 = lexer.tokenize("test\\|pipe", ctx);
    EXPECT_EQ(ts7[0].value, "test|pipe");
}

TEST(LexerTest, Operators) {
    ExecutionContext ctx;
    Lexer lexer;

    auto ts1 = lexer.tokenize("a|b", ctx);
    ASSERT_GE(ts1.size(), 4U);
    EXPECT_EQ(ts1[0].type, TokenType::WORD);
    EXPECT_EQ(ts1[1].type, TokenType::OP_PIPE);
    EXPECT_EQ(ts1[2].type, TokenType::WORD);

    auto ts2 = lexer.tokenize("x=1", ctx);
    ASSERT_GE(ts2.size(), 4U);
    EXPECT_EQ(ts2[0].type, TokenType::WORD);
    EXPECT_EQ(ts2[1].type, TokenType::OP_ASSIGN);

    auto ts3 = lexer.tokenize("$VAR=value", ctx);
    ASSERT_GE(ts3.size(), 5U);
    EXPECT_EQ(ts3[0].type, TokenType::OP_DOLLAR);
    EXPECT_EQ(ts3[1].value, "VAR");
    EXPECT_EQ(ts3[2].type, TokenType::OP_ASSIGN);
    EXPECT_EQ(ts3[3].value, "value");
}

TEST(LexerTest, QuotedCharacters) {
    ExecutionContext ctx;
    Lexer lexer;

    auto ts1 = lexer.tokenize("echo \"'\"", ctx);
    ASSERT_GE(ts1.size(), 2U);
    EXPECT_EQ(ts1[0].value, "echo");
    EXPECT_EQ(ts1[1].value, "'");

    auto ts2 = lexer.tokenize("echo '\"'", ctx);
    ASSERT_GE(ts2.size(), 2U);
    EXPECT_EQ(ts2[0].value, "echo");
    EXPECT_EQ(ts2[1].value, "\"");
}

TEST(LexerTest, UnclosedQuotes) {
    ExecutionContext ctx;
    Lexer lexer;

    EXPECT_THROW(
        lexer.tokenize("echo '", ctx),
        std::runtime_error
    );

    EXPECT_THROW(
        lexer.tokenize("echo \"", ctx),
        std::runtime_error
    );
}

TEST(LexerTest, UnclosedQuotesMultiple) {
    ExecutionContext ctx;
    Lexer lexer;

    EXPECT_THROW(
        lexer.tokenize("echo \"hello 'world", ctx),
        std::runtime_error
    );

    EXPECT_THROW(
        lexer.tokenize("echo \"hello world", ctx),
        std::runtime_error
    );

    EXPECT_THROW(
        lexer.tokenize("echo \"hello 'world", ctx),
        std::runtime_error
    );
}

}  // namespace
}  // namespace fluffy_tribble
