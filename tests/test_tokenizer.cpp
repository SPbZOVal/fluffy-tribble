/** @file test_tokenizer.cpp
 *  Юнит-тесты для токенайзера.
 */

#include "tokenizer.hpp"
#include <gtest/gtest.h>

using namespace cli;

TEST(TokenizerTest, EmptyLineYieldsEmptyList) {
  EXPECT_TRUE(tokenize("").empty());
  EXPECT_TRUE(tokenize("   ").empty());
  EXPECT_TRUE(tokenize("\t\n  ").empty());
}

TEST(TokenizerTest, SingleWord) {
  auto t = tokenize("echo");
  ASSERT_EQ(t.size(), 1u);
  EXPECT_EQ(t[0], "echo");
}

TEST(TokenizerTest, SeveralWords) {
  auto t = tokenize("echo hello world");
  ASSERT_EQ(t.size(), 3u);
  EXPECT_EQ(t[0], "echo");
  EXPECT_EQ(t[1], "hello");
  EXPECT_EQ(t[2], "world");
}

TEST(TokenizerTest, DoubleQuotesMakeOneArgument) {
  auto t = tokenize("echo \"hello world\"");
  ASSERT_EQ(t.size(), 2u);
  EXPECT_EQ(t[0], "echo");
  EXPECT_EQ(t[1], "hello world");
}

TEST(TokenizerTest, SingleQuotesMakeOneArgument) {
  auto t = tokenize("echo 'hello world'");
  ASSERT_EQ(t.size(), 2u);
  EXPECT_EQ(t[0], "echo");
  EXPECT_EQ(t[1], "hello world");
}

TEST(TokenizerTest, MixedQuotedAndUnquoted) {
  auto t = tokenize("cat \"file name.txt\"");
  ASSERT_EQ(t.size(), 2u);
  EXPECT_EQ(t[0], "cat");
  EXPECT_EQ(t[1], "file name.txt");
}

TEST(TokenizerTest, QuotedEmptyString) {
  auto t = tokenize("echo \"\"");
  ASSERT_EQ(t.size(), 2u);
  EXPECT_EQ(t[0], "echo");
  EXPECT_EQ(t[1], "");
}

TEST(TokenizerTest, MultipleQuotedSegments) {
  auto t = tokenize("a \"b c\" d 'e f' g");
  ASSERT_EQ(t.size(), 5u);
  EXPECT_EQ(t[0], "a");
  EXPECT_EQ(t[1], "b c");
  EXPECT_EQ(t[2], "d");
  EXPECT_EQ(t[3], "e f");
  EXPECT_EQ(t[4], "g");
}

TEST(TokenizerTest, LeadingAndTrailingSpaces) {
  auto t = tokenize("  echo foo  ");
  ASSERT_EQ(t.size(), 2u);
  EXPECT_EQ(t[0], "echo");
  EXPECT_EQ(t[1], "foo");
}
