#ifndef fluffy_tribble_PIPE_EXECUTOR_HPP
#define fluffy_tribble_PIPE_EXECUTOR_HPP

#include <iosfwd>
#include <vector>
#include "execution_context.hpp"
#include "parsed_command.hpp"

namespace fluffy_tribble {

/**
 * Последовательно выполняет команды пайплайна (в первой части — одна команда).
 * Перенаправление потоков между командами не реализуется в этой части.
 */
class PipeExecutor {
public:
    /**
     * Выполняет все команды пайплайна по порядку.
     * При установке ctx.is_exit() (команда exit) оставшиеся команды не
     * запускаются.
     * @param pipe Пайплайн (вектор команд).
     * @param input Входной поток для первой команды.
     * @param output Выходной поток.
     * @param error Поток ошибок.
     * @param ctx Глобальный контекст выполнения.
     */
    static void execute(
        const Pipe &pipe,
        std::istream &input,
        std::ostream &output,
        std::ostream &error,
        ExecutionContext &ctx
    );
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_PIPE_EXECUTOR_HPP
