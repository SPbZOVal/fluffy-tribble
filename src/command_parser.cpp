#include "command_parser.hpp"
#include <string>
#include "command_manager.hpp"

namespace fluffy_tribble {

Pipe CommandParser::parse(const TokenStream &tokens) {
    Pipe pipe;
    std::vector<std::string> words;

    for (TokenStream::size_type i = 0; i < tokens.size(); ++i) {
        const Token &t = tokens[i];
        if (t.type == TokenType::EOF_) {
            break;
        }
        if (t.type == TokenType::OP_PIPE) {
            if (!words.empty()) {
                ParsedCommand cmd;
                cmd.name = std::move(words[0]);
                words.erase(words.begin());
                cmd.args.assign(words.begin(), words.end());
                cmd.id = CommandManager::get_command_id(cmd.name, cmd.args);
                pipe.push_back(std::move(cmd));
                words.clear();
            }
            continue;
        }
        if (t.type == TokenType::OP_ASSIGN && !words.empty()) {
            std::string var_name = std::move(words[0]);
            words.erase(words.begin());
            std::string value;
            if (i + 1 < tokens.size() &&
                tokens[i + 1].type == TokenType::WORD) {
                value = tokens[i + 1].value;
                ++i;
            }
            ParsedCommand assign;
            assign.name = std::move(var_name);
            assign.args.push_back(std::move(value));
            assign.id = CommandID::ASSIGN;
            pipe.push_back(std::move(assign));
            words.clear();
            continue;
        }
        if (t.type == TokenType::WORD) {
            words.push_back(t.value);
        }
    }

    if (!words.empty()) {
        ParsedCommand cmd;
        cmd.name = std::move(words[0]);
        words.erase(words.begin());
        cmd.args.assign(words.begin(), words.end());
        cmd.id = CommandManager::get_command_id(cmd.name, cmd.args);
        pipe.push_back(std::move(cmd));
    }

    return pipe;
}

}  // namespace fluffy_tribble
