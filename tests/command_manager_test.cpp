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

}  // namespace
}  // namespace fluffy_tribble
