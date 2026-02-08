#include "lexer.hpp"
#include "token.hpp"
#include <gtest/gtest.h>

namespace lka {
namespace {

TEST(LexerTest, EmptyInput) {
    Lexer lexer;
    TokenStream ts = lexer.tokenize("");
    ASSERT_EQ(ts.size(), 1u);
    EXPECT_EQ(ts[0].type, TokenType::EOF_);
    EXPECT_TRUE(ts[0].value.empty());
}

TEST(LexerTest, SingleWord) {
    Lexer lexer;
    TokenStream ts = lexer.tokenize("hello");
    ASSERT_GE(ts.size(), 2u);
    EXPECT_EQ(ts[0].type, TokenType::WORD);
    EXPECT_EQ(ts[0].value, "hello");
    EXPECT_EQ(ts.back().type, TokenType::EOF_);
}

TEST(LexerTest, TwoWords) {
    Lexer lexer;
    TokenStream ts = lexer.tokenize("echo hello");
    ASSERT_GE(ts.size(), 3u);
    EXPECT_EQ(ts[0].type, TokenType::WORD);
    EXPECT_EQ(ts[0].value, "echo");
    EXPECT_EQ(ts[1].type, TokenType::WORD);
    EXPECT_EQ(ts[1].value, "hello");
    EXPECT_EQ(ts.back().type, TokenType::EOF_);
}

TEST(LexerTest, SingleQuotes) {
    Lexer lexer;
    TokenStream ts = lexer.tokenize("'one arg'");
    ASSERT_GE(ts.size(), 2u);
    EXPECT_EQ(ts[0].type, TokenType::WORD);
    EXPECT_EQ(ts[0].value, "one arg");
    EXPECT_EQ(ts.back().type, TokenType::EOF_);
}

TEST(LexerTest, DoubleQuotes) {
    Lexer lexer;
    TokenStream ts = lexer.tokenize("\"two words\"");
    ASSERT_GE(ts.size(), 2u);
    EXPECT_EQ(ts[0].type, TokenType::WORD);
    EXPECT_EQ(ts[0].value, "two words");
    EXPECT_EQ(ts.back().type, TokenType::EOF_);
}

TEST(LexerTest, PipeOperator) {
    Lexer lexer;
    TokenStream ts = lexer.tokenize("a|b");
    ASSERT_GE(ts.size(), 4u);
    EXPECT_EQ(ts[0].type, TokenType::WORD);
    EXPECT_EQ(ts[0].value, "a");
    EXPECT_EQ(ts[1].type, TokenType::OP_PIPE);
    EXPECT_EQ(ts[1].value, "|");
    EXPECT_EQ(ts[2].type, TokenType::WORD);
    EXPECT_EQ(ts[2].value, "b");
    EXPECT_EQ(ts.back().type, TokenType::EOF_);
}

TEST(LexerTest, AssignOperator) {
    Lexer lexer;
    TokenStream ts = lexer.tokenize("x=1");
    ASSERT_GE(ts.size(), 4u);
    EXPECT_EQ(ts[0].type, TokenType::WORD);
    EXPECT_EQ(ts[0].value, "x");
    EXPECT_EQ(ts[1].type, TokenType::OP_ASSIGN);
    EXPECT_EQ(ts[2].type, TokenType::WORD);
    EXPECT_EQ(ts[2].value, "1");
    EXPECT_EQ(ts.back().type, TokenType::EOF_);
}

}  // namespace
}  // namespace lka
