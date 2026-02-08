#ifndef LKA_COMMAND_EXECUTOR_HPP
#define LKA_COMMAND_EXECUTOR_HPP

#include "execution_context.hpp"
#include "parsed_command.hpp"
#include <iosfwd>

namespace lka {

/**
 * Выполняет одну команду: получает реализацию из CommandManager,
 * вызывает встроенную run или внешнюю программу.
 */
class CommandExecutor {
public:
    /**
     * Выполняет одну команду.
     * @param cmd Разобранная команда (имя, аргументы, id).
     * @param input Входной поток (для встроенных, читающих из stdin).
     * @param output Выходной поток.
     * @param error Поток ошибок.
     * @param ctx Контекст (окружение, exit-флаг и т.д.).
     */
    static void execute(const ParsedCommand& cmd,
                        std::istream& input,
                        std::ostream& output,
                        std::ostream& error,
                        ExecutionContext& ctx);
};

}  // namespace lka

#endif  // LKA_COMMAND_EXECUTOR_HPP
