#ifndef fluffy_tribble_LEXER_HPP
#define fluffy_tribble_LEXER_HPP

#include "execution_context.hpp"
#include "token.hpp"

namespace fluffy_tribble {

/**
 * Лексер: разбивает входную строку на токены с учётом quoting.
 * Одинарные и двойные кавычки обрабатываются; строка в кавычках — один аргумент
 * (WORD). Подстановка переменных окружения выполняется во время токенизации.
 */
class Lexer {
public:
    /**
     * Разбивает входную строку на токены с подстановкой переменных.
     * В одинарных кавычках (full quoting) переменные не подставляются.
     * В двойных кавычках (weak quoting) переменные подставляются.
     * @param input Входная строка (одна строка ввода пользователя).
     * @param ctx Контекст выполнения для подстановки переменных.
     * @return Поток токенов (включая EOF_ в конце).
     */
    TokenStream tokenize(const std::string &input, ExecutionContext &ctx);
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_LEXER_HPP