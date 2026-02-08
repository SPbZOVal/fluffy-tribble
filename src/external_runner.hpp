/** @file external_runner.hpp
 *  Запуск внешней программы с передачей окружения и перенаправлением stdout/stderr.
 */

#ifndef FLUFFY_TRIBBLE_EXTERNAL_RUNNER_HPP
#define FLUFFY_TRIBBLE_EXTERNAL_RUNNER_HPP

#include <iosfwd>
#include <string>
#include <vector>

namespace cli {

/** Запускает внешнюю программу с заданными аргументами и текущим окружением.
 *  stdout и stderr дочернего процесса выводятся в out и err соответственно.
 *  @param args args[0] — путь/имя программы, остальные — аргументы
 *  @param out поток для stdout дочернего процесса
 *  @param err поток для stderr дочернего процесса
 *  @return код возврата дочернего процесса (или -1 при ошибке запуска)
 */
int run_external(const std::vector<std::string>& args,
                 std::ostream& out,
                 std::ostream& err);

}  // namespace cli

#endif  // FLUFFY_TRIBBLE_EXTERNAL_RUNNER_HPP
