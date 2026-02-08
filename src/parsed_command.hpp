#ifndef LKA_PARSED_COMMAND_HPP
#define LKA_PARSED_COMMAND_HPP

#include "command_id.hpp"
#include <string>
#include <vector>

namespace lka {

/**
 * Одна команда после разбора: имя, аргументы и тип (встроенная/внешняя/присваивание/exit).
 */
struct ParsedCommand {
    /** Имя команды (или имя переменной при присваивании). */
    std::string name;
    /** Аргументы команды (пустой вектор для команды без аргументов). */
    std::vector<std::string> args;
    /** Тип команды для диспетчеризации выполнения. */
    CommandID id = CommandID::EXTERNAL;
};

/**
 * Пайплайн: последовательность команд (в первой части — одна команда).
 */
using Pipe = std::vector<ParsedCommand>;

}  // namespace lka

#endif  // LKA_PARSED_COMMAND_HPP
