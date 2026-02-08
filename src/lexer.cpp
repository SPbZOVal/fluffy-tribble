#include "lexer.hpp"
#include <cctype>
#include <utility>
#include "token.hpp"

namespace fluffy_tribble {

namespace {

constexpr bool is_special_char(char c) {
    return c == ' ' || c == '|' || c == '$' || c == '"' || c == '\'' ||
           c == '\\';
}

constexpr bool is_dq_escape(char c) {
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

}  // namespace

TokenStream Lexer::tokenize(const std::string &input) {
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

    for (std::string::size_type i = 0; i <= input.size(); ++i) {
        const char c = (i < input.size()) ? input[i] : '\0';

        if (escaping) {
            if (in_double) {
                if (!is_dq_escape(c)) {
                    word += '\\';
                }
                word += c;
            } else {
                word += c;
            }
            escaping = false;
            continue;
        }

        if (c == '\\') {
            const bool do_escape =
                in_double || (i + 1 < input.size() &&
                              is_special_char(input[i + 1]) && !in_single);
            if (do_escape) {
                escaping = true;
            } else {
                word += c;
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

        if (c == '$' && !in_single && !in_double) {
            flush_word();
            out.push_back(Token{.type = TokenType::OP_DOLLAR, .value = "$"});
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c)) && !in_single &&
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
