#include "lexer.hpp"
#include <cctype>
#include <utility>
#include "token.hpp"

namespace fluffy_tribble {

namespace {

constexpr bool is_special_char(char c) {
    return c == ' ' || c == '|' || c == '$' || c == '"' || c == '\'' ||
           c == '\\' || c == '=';
}

constexpr bool is_dq_escape(char c) {
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == 'n';
}

void handle_escape(char c, bool in_double, std::string &word) {
    if (in_double) {
        if (is_dq_escape(c)) {
            if (c == 'n') {
                word += '\n';
            } else {
                word += c;
            }
        } else {
            word += '\\';
            word += c;
        }
    } else {
        word += c;
    }
}

std::size_t handle_dollar(
    const std::string &input,
    std::size_t i,
    bool in_double,
    std::string &word,
    TokenStream &out,
    ExecutionContext &ctx,
    const auto &flush_word
) {
    if (i + 1 < input.size() &&
        (std::isalnum(static_cast<unsigned char>(input[i + 1])) ||
         input[i + 1] == '_')) {
        std::string var_name;
        std::size_t j = i + 1;
        while (j < input.size() &&
               (std::isalnum(static_cast<unsigned char>(input[j])) ||
                input[j] == '_')) {
            var_name += input[j];
            ++j;
        }

        bool is_assignment =
            (j < input.size() && input[j] == '=' && !in_double);

        if (is_assignment) {
            flush_word();
            out.push_back(Token{.type = TokenType::OP_DOLLAR, .value = "$"});
            return i;
        } else {
            auto it = ctx.env().find(var_name);
            if (it != ctx.env().end()) {
                word += it->second;
            }
            return j - 1;
        }
    } else if (!in_double) {
        flush_word();
        out.push_back(Token{.type = TokenType::OP_DOLLAR, .value = "$"});
    }

    return i;
}

}  // namespace

TokenStream Lexer::tokenize(const std::string &input, ExecutionContext &ctx) {
    TokenStream out;
    std::string word;
    bool in_single = false;
    bool in_double = false;
    bool escaping = false;

    const auto flush_word = [&out, &word]() {
        if (!word.empty()) {
            out.push_back(
                Token{.type = TokenType::WORD, .value = std::move(word)}
            );
            word.clear();
        }
    };

    for (std::size_t i = 0; i <= input.size(); ++i) {
        const char c = (i < input.size()) ? input[i] : '\0';

        if (escaping) {
            handle_escape(c, in_double, word);
            escaping = false;
            continue;
        }

        if (c == '\\') {
            if (in_single) {
                word += c;
            } else if (in_double) {
                escaping = true;
            } else {
                if (i + 1 < input.size() && is_special_char(input[i + 1])) {
                    escaping = true;
                } else {
                    word += c;
                }
            }
            continue;
        }

        if (c == '\'' && !in_double) {
            in_single = !in_single;
            continue;
        }

        if (c == '"' && !in_single) {
            in_double = !in_double;
            continue;
        }

        if (c == '$' && !in_single) {
            i = handle_dollar(input, i, in_double, word, out, ctx, flush_word);
            continue;
        }

        if (c == '|' && !in_single && !in_double) {
            flush_word();
            out.push_back(Token{.type = TokenType::OP_PIPE, .value = "|"});
            continue;
        }

        if (c == '=' && !in_single && !in_double) {
            flush_word();
            out.push_back(Token{.type = TokenType::OP_ASSIGN, .value = "="});
            continue;
        }

        if ((std::isspace(static_cast<unsigned char>(c)) != 0) && !in_single &&
            !in_double) {
            flush_word();
            continue;
        }

        if (c == '\0') {
            flush_word();
            break;
        }

        word += c;
    }

    out.push_back(Token{.type = TokenType::EOF_, .value = ""});

    return out;
}

}  // namespace fluffy_tribble
