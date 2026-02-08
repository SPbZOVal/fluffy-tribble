#ifndef fluffy_tribble_TOKEN_HPP
#define fluffy_tribble_TOKEN_HPP

#include <string>
#include <vector>

namespace fluffy_tribble {

/** Тип токена, выдаваемого лексером. */
enum class TokenType {
    WORD,          ///< Слово (идентификатор или аргумент)
    QUOTE_SINGLE,  ///< Одиночная кавычка '
    QUOTE_DOUBLE,  ///< Двойная кавычка "
    OP_PIPE,       ///< Оператор пайпа |
    OP_ASSIGN,     ///< Оператор присваивания =
    OP_DOLLAR,     ///< Символ переменной $
    SPECIAL,       ///< Специальный символ
    EOF_           ///< Конец ввода
};

/**
 * Один токен: тип и строковое значение (для WORD — содержимое).
 */
struct Token {
    TokenType type;
    std::string value;
};

/**
 * Поток токенов — результат работы лексера (вектор токенов).
 */
using TokenStream = std::vector<Token>;

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_TOKEN_HPP
