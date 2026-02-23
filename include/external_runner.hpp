#ifndef fluffy_tribble_EXTERNAL_RUNNER_HPP
#define fluffy_tribble_EXTERNAL_RUNNER_HPP

#include <iosfwd>
#include <string>
#include <vector>
#include "execution_context.hpp"

namespace fluffy_tribble {

/**
 * Запуск внешней программы с заданными аргументами и окружением из контекста.
 * Потоки ввода/вывода перенаправляются с помощью dup2.
 */
class ExternalRunner {
public:
    /**
     * Запускает внешнюю программу.
     * @param name Имя или путь к программе (при отсутствии / ищется в PATH).
     * @param args Аргументы командной строки (argv[1..]).
     * @param input Входной поток.
     * @param output Выходной поток.
     * @param error Поток ошибок.
     * @param ctx Контекст (окружение берётся из ctx.env()).
     * @return Код возврата процесса.
     */
    static int run(
        const std::string &name,
        const std::vector<std::string> &args,
        std::istream &input,
        std::ostream &output,
        std::ostream &error,
        ExecutionContext &ctx
    );
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_EXTERNAL_RUNNER_HPP
