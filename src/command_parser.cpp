#include "command_parser.hpp"
#include <string>
#include "command_id.hpp"
#include "command_manager.hpp"
#include "token.hpp"

namespace fluffy_tribble {

namespace {
bool should_insert_name(CommandID id) {
    return id == CommandID::EXTERNAL || id == CommandID::GREP;
}
}  // namespace

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
                cmd.id = CommandManager::get_command_id(cmd.name);
                if (should_insert_name(cmd.id)) {
                    cmd.args.insert(cmd.args.begin(), cmd.name);
                }
                pipe.push_back(std::move(cmd));
                words.clear();
            }
            continue;
        }
        if (t.type == TokenType::OP_DOLLAR && i + 2 < tokens.size() &&
            tokens[i + 1].type == TokenType::WORD &&
            tokens[i + 2].type == TokenType::OP_ASSIGN) {
            std::string var_name = tokens[i + 1].value;
            std::string value;
            if (i + 3 < tokens.size() &&
                tokens[i + 3].type == TokenType::WORD) {
                value = tokens[i + 3].value;
                i += 3;
            } else {
                i += 2;
            }
            ParsedCommand assign;
            assign.name = std::move(var_name);
            assign.args.push_back(std::move(value));
            assign.id = CommandID::ASSIGN;
            pipe.push_back(std::move(assign));
            continue;
        }
        if (t.type == TokenType::WORD) {
            if (i + 2 < tokens.size() &&
                tokens[i + 1].type == TokenType::OP_ASSIGN &&
                tokens[i + 2].type == TokenType::WORD) {
                words.push_back(
                    tokens[i].value + tokens[i + 1].value + tokens[i + 2].value
                );
                i += 2;
                continue;
            }
            words.push_back(t.value);
        }
    }

    if (!words.empty()) {
        ParsedCommand cmd;
        cmd.name = std::move(words[0]);
        words.erase(words.begin());
        cmd.args.assign(words.begin(), words.end());
        cmd.id = CommandManager::get_command_id(cmd.name);
        if (should_insert_name(cmd.id)) {
            cmd.args.insert(cmd.args.begin(), cmd.name);
        }
        pipe.push_back(std::move(cmd));
    }

    return pipe;
}

}  // namespace fluffy_tribble
