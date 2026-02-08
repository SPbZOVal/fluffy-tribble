#include "command_id.hpp"
#include "command_manager.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace lka {
namespace {

TEST(CommandManagerTest, Exit) {
    std::vector<std::string> args;
    EXPECT_EQ(CommandManager::get_command_id("exit", args), CommandID::EXIT);
}

TEST(CommandManagerTest, Builtins) {
    std::vector<std::string> args;
    EXPECT_EQ(CommandManager::get_command_id("cat", args), CommandID::BUILTIN);
    EXPECT_EQ(CommandManager::get_command_id("echo", args), CommandID::BUILTIN);
    EXPECT_EQ(CommandManager::get_command_id("wc", args), CommandID::BUILTIN);
    EXPECT_EQ(CommandManager::get_command_id("pwd", args), CommandID::BUILTIN);
}

TEST(CommandManagerTest, External) {
    std::vector<std::string> args;
    EXPECT_EQ(CommandManager::get_command_id("ls", args), CommandID::EXTERNAL);
    EXPECT_EQ(CommandManager::get_command_id("custom_tool", args), CommandID::EXTERNAL);
}

TEST(CommandManagerTest, IsBuiltin) {
    EXPECT_TRUE(CommandManager::is_builtin("cat"));
    EXPECT_TRUE(CommandManager::is_builtin("echo"));
    EXPECT_TRUE(CommandManager::is_builtin("wc"));
    EXPECT_TRUE(CommandManager::is_builtin("pwd"));
    EXPECT_FALSE(CommandManager::is_builtin("exit"));
    EXPECT_FALSE(CommandManager::is_builtin("ls"));
}

}  // namespace
}  // namespace lka
