#include "command_manager.hpp"
#include <cctype>
#include <string>

namespace fluffy_tribble {

namespace {

const char *const BUILTIN_NAMES[] = {"cat", "echo", "wc", "pwd"};
constexpr std::size_t NUM_BUILTINS =
    sizeof(BUILTIN_NAMES) / sizeof(BUILTIN_NAMES[0]);

bool string_equals_ignore_case(const std::string &a, const char *b) {
    if (a.size() != std::char_traits<char>::length(b)) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

}  // namespace

CommandID CommandManager::
    get_command_id(const std::string &name, const std::vector<std::string> &) {
    if (name.empty()) {
        return CommandID::EXTERNAL;
    }
    if (string_equals_ignore_case(name, "exit")) {
        return CommandID::EXIT;
    }
    for (std::size_t i = 0; i < NUM_BUILTINS; ++i) {
        if (string_equals_ignore_case(name, BUILTIN_NAMES[i])) {
            return CommandID::BUILTIN;
        }
    }
    return CommandID::EXTERNAL;
}

bool CommandManager::is_builtin(const std::string &name) {
    for (std::size_t i = 0; i < NUM_BUILTINS; ++i) {
        if (string_equals_ignore_case(name, BUILTIN_NAMES[i])) {
            return true;
        }
    }
    return false;
}

}  // namespace fluffy_tribble
