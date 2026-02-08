#ifndef LKA_PIPE_EXECUTOR_HPP
#define LKA_PIPE_EXECUTOR_HPP

#include "execution_context.hpp"
#include "parsed_command.hpp"
#include <iosfwd>
#include <vector>

namespace lka {

/**
 * Последовательно выполняет команды пайплайна (в первой части — одна команда).
 * Перенаправление потоков между командами не реализуется в этой части.
 */
class PipeExecutor {
public:
    /**
     * Выполняет все команды пайплайна по порядку.
     * При установке ctx.is_exit() (команда exit) оставшиеся команды не запускаются.
     * @param pipe Пайплайн (вектор команд).
     * @param input Входной поток для первой команды.
     * @param output Выходной поток.
     * @param error Поток ошибок.
     * @param ctx Глобальный контекст выполнения.
     */
    static void execute(const Pipe& pipe,
                        std::istream& input,
                        std::ostream& output,
                        std::ostream& error,
                        ExecutionContext& ctx);
};

}  // namespace lka

#endif  // LKA_PIPE_EXECUTOR_HPP
