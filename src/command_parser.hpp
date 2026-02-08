#ifndef LKA_COMMAND_PARSER_HPP
#define LKA_COMMAND_PARSER_HPP

#include "parsed_command.hpp"
#include "token.hpp"
#include <vector>

namespace lka {

/**
 * Преобразует поток токенов в пайплайн команд (набор ParsedCommand).
 * Присваивание (name=value) трактуется как отдельная команда с id ASSIGN.
 */
class CommandParser {
public:
    /**
     * Разбирает поток токенов в пайплайн.
     * Пайпы (|) в этой части не обрабатываются — все слова до EOF образуют одну команду.
     * @param tokens Результат работы лексера (должен заканчиваться EOF_).
     * @return Пайплайн (вектор команд; без пайпов — одна команда).
     */
    Pipe parse(const TokenStream& tokens);
};

}  // namespace lka

#endif  // LKA_COMMAND_PARSER_HPP
