#include "command_manager.hpp"
#include <gtest/gtest.h>
#include "command_id.hpp"

namespace fluffy_tribble {
namespace {

TEST(CommandManagerTest, Exit) {
    EXPECT_EQ(CommandManager::get_command_id("exit"), CommandID::EXIT);
}

TEST(CommandManagerTest, Builtins) {
    EXPECT_EQ(CommandManager::get_command_id("cat"), CommandID::CAT);
    EXPECT_EQ(CommandManager::get_command_id("echo"), CommandID::ECHO);
    EXPECT_EQ(CommandManager::get_command_id("wc"), CommandID::WC);
    EXPECT_EQ(CommandManager::get_command_id("pwd"), CommandID::PWD);
}

TEST(CommandManagerTest, External) {
    EXPECT_EQ(CommandManager::get_command_id("ls"), CommandID::EXTERNAL);
    EXPECT_EQ(
        CommandManager::get_command_id("custom_tool"), CommandID::EXTERNAL
    );
}

TEST(CommandManagerTest, RegisterCommand) {
    CommandManager::register_command("list", CommandID::CAT);
    EXPECT_EQ(CommandManager::get_command_id("list"), CommandID::CAT);
}

TEST(CommandManagerTest, ErrorUnknownCommand) {
    // Unknown command should return EXTERNAL
    EXPECT_EQ(
        CommandManager::get_command_id("unknown_cmd_xyz"), CommandID::EXTERNAL
    );
}

TEST(CommandManagerTest, ErrorEmptyCommandName) {
    // Empty string should return EXTERNAL
    EXPECT_EQ(CommandManager::get_command_id(""), CommandID::EXTERNAL);
}

TEST(CommandManagerTest, ErrorWhitespaceCommand) {
    // Whitespace should return EXTERNAL
    EXPECT_EQ(CommandManager::get_command_id("   "), CommandID::EXTERNAL);
}

TEST(CommandManagerTest, ErrorDuplicateRegister) {
    // Registering same name twice should use latest
    CommandManager::register_command("test_cmd", CommandID::CAT);
    EXPECT_EQ(CommandManager::get_command_id("test_cmd"), CommandID::CAT);
    CommandManager::register_command("test_cmd", CommandID::ECHO);
    EXPECT_EQ(CommandManager::get_command_id("test_cmd"), CommandID::ECHO);
}

TEST(CommandManagerTest, ErrorNullptr) {
    // Getting function pointer for unknown command
    auto fn = CommandManager::get_command_fn(CommandID::EXTERNAL);
    EXPECT_EQ(fn, nullptr);
}

}  // namespace
}  // namespace fluffy_tribble
