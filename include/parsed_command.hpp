#ifndef fluffy_tribble_PARSED_COMMAND_HPP
#define fluffy_tribble_PARSED_COMMAND_HPP

#include <string>
#include <vector>
#include "command_id.hpp"

namespace fluffy_tribble {

/**
 * Одна команда после разбора: имя, аргументы и тип
 * (встроенная/внешняя/присваивание/exit).
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

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_PARSED_COMMAND_HPP
