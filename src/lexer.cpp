#include "lexer.hpp"
#include "token.hpp"
#include <cctype>
#include <utility>

namespace lka {

namespace {

bool is_special(char c) {
    return c == '|' || c == '=' || c == '$' || c == '\0';
}

}  // namespace

TokenStream Lexer::tokenize(const std::string& input) {
    TokenStream out;
    std::string::size_type i = 0;
    const auto n = input.size();

    while (i < n) {
        const char c = input[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            ++i;
            continue;
        }

        if (c == '\'') {
            std::string word;
            ++i;
            while (i < n && input[i] != '\'') {
                word += input[i++];
            }
            if (i < n) ++i;  // skip closing '
            out.push_back(Token{TokenType::WORD, std::move(word)});
            continue;
        }

        if (c == '"') {
            std::string word;
            ++i;
            while (i < n && input[i] != '"') {
                word += input[i++];
            }
            if (i < n) ++i;
            out.push_back(Token{TokenType::WORD, std::move(word)});
            continue;
        }

        if (c == '|') {
            out.push_back(Token{TokenType::OP_PIPE, "|"});
            ++i;
            continue;
        }

        if (c == '=') {
            out.push_back(Token{TokenType::OP_ASSIGN, "="});
            ++i;
            continue;
        }

        if (c == '$') {
            out.push_back(Token{TokenType::OP_DOLLAR, "$"});
            ++i;
            continue;
        }

        if (is_special(c)) {
            out.push_back(Token{TokenType::SPECIAL, std::string(1, c)});
            ++i;
            continue;
        }

        // WORD: последовательность непробельных символов (не кавычка, не оператор)
        std::string word;
        while (i < n && !std::isspace(static_cast<unsigned char>(input[i])) &&
               input[i] != '\'' && input[i] != '"' && input[i] != '|' &&
               input[i] != '=' && input[i] != '$') {
            word += input[i++];
        }
        if (!word.empty()) {
            out.push_back(Token{TokenType::WORD, std::move(word)});
        }
    }

    out.push_back(Token{TokenType::EOF_, ""});
    return out;
}

}  // namespace lka
