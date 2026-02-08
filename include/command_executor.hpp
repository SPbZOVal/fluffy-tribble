#ifndef fluffy_tribble_COMMAND_EXECUTOR_HPP
#define fluffy_tribble_COMMAND_EXECUTOR_HPP

#include <iosfwd>
#include "execution_context.hpp"
#include "parsed_command.hpp"

namespace fluffy_tribble {

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
    static void execute(
        const ParsedCommand &cmd,
        std::istream &input,
        std::ostream &output,
        std::ostream &error,
        ExecutionContext &ctx
    );
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_COMMAND_EXECUTOR_HPP
