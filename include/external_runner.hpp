#ifndef fluffy_tribble_EXTERNAL_RUNNER_HPP
#define fluffy_tribble_EXTERNAL_RUNNER_HPP

#include <string>
#include <vector>
#include "execution_context.hpp"

namespace fluffy_tribble {

/**
 * Запуск внешней программы с заданными аргументами и окружением из контекста.
 * Стандартные потоки процесса наследуются от родителя (stdout/stderr идут в
 * консоль).
 */
class ExternalRunner {
public:
    /**
     * Запускает внешнюю программу.
     * @param name Имя или путь к программе (при отсутствии / ищется в PATH).
     * @param args Аргументы командной строки (argv[1..]).
     * @param ctx Контекст (окружение берётся из ctx.env()).
     * @return Код возврата процесса (как от waitpid).
     */
    static int run(
        const std::string &name,
        const std::vector<std::string> &args,
        ExecutionContext &ctx
    );
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_EXTERNAL_RUNNER_HPP
