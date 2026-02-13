#ifndef fluffy_tribble_LEXER_HPP
#define fluffy_tribble_LEXER_HPP

#include "token.hpp"

namespace fluffy_tribble {

/**
 * Лексер: разбивает входную строку на токены с учётом quoting.
 * Одинарные и двойные кавычки обрабатываются; строка в кавычках — один аргумент
 * (WORD). Подстановка переменных окружения в этой части не реализуется.
 */
class Lexer {
public:
    /**
     * Разбивает входную строку на токены.
     * @param input Входная строка (одна строка ввода пользователя).
     * @return Поток токенов (включая EOF_ в конце).
     */
    TokenStream tokenize(const std::string &input);
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_LEXER_HPP
