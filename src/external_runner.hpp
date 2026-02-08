#ifndef LKA_EXTERNAL_RUNNER_HPP
#define LKA_EXTERNAL_RUNNER_HPP

#include "execution_context.hpp"
#include <string>
#include <vector>

namespace lka {

/**
 * Запуск внешней программы с заданными аргументами и окружением из контекста.
 * Стандартные потоки процесса наследуются от родителя (stdout/stderr идут в консоль).
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
    static int run(const std::string& name,
                   const std::vector<std::string>& args,
                   ExecutionContext& ctx);
};

}  // namespace lka

#endif  // LKA_EXTERNAL_RUNNER_HPP
