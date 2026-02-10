#include "command_manager.hpp"
#include <cctype>
#include <string>
#include <string_view>
#include <unordered_map>
#include "builtins.hpp"
#include "execution_context.hpp"

namespace fluffy_tribble {

namespace {

std::unordered_map<std::string, CommandID> &name_to_id() {
    static std::unordered_map<std::string, CommandID> map;
    return map;
}

std::string to_lower(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        out += static_cast<char>(std::tolower(c));
    }
    return out;
}

bool init_default_commands() {
    auto &m = name_to_id();
    if (!m.empty()) {
        return true;
    }
    m[to_lower("cat")] = CommandID::CAT;
    m[to_lower("echo")] = CommandID::ECHO;
    m[to_lower("wc")] = CommandID::WC;
    m[to_lower("pwd")] = CommandID::PWD;
    m[to_lower("exit")] = CommandID::EXIT;
    return true;
}

}  // namespace

CommandID CommandManager::get_command_id(const std::string &name) {
    static bool once = init_default_commands();
    (void)once;
    auto it = name_to_id().find(to_lower(name));
    if (it == name_to_id().end()) {
        return CommandID::EXTERNAL;
    }
    return it->second;
}

void CommandManager::register_command(const std::string &name, CommandID id) {
    init_default_commands();
    name_to_id()[to_lower(name)] = id;
}

CommandManager::CommandFn CommandManager::get_command_fn(CommandID id) {
    switch (id) {
        case CommandID::CAT:
            return &run<CommandID::CAT>;
        case CommandID::ECHO:
            return &run<CommandID::ECHO>;
        case CommandID::WC:
            return &run<CommandID::WC>;
        case CommandID::PWD:
            return &run<CommandID::PWD>;
        case CommandID::EXIT:
            return &run<CommandID::EXIT>;
        default:
            return nullptr;
    }
}

}  // namespace fluffy_tribble
