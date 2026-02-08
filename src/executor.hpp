/** @file executor.hpp
 *  Исполнитель команд: маршрутизация на встроенные команды или внешний процесс.
 */

#ifndef FLUFFY_TRIBBLE_EXECUTOR_HPP
#define FLUFFY_TRIBBLE_EXECUTOR_HPP

#include "builtins.hpp"
#include "tokenizer.hpp"
#include <ostream>

namespace cli {

/** Выполняет одну команду по списку аргументов.
 *  Первый аргумент — имя команды; встроенные (cat, echo, wc, pwd, exit) выполняются
 *  внутри процесса, остальные — запуском внешней программы с текущим окружением.
 *  @param args список аргументов (результат tokenize)
 *  @param out поток для stdout
 *  @param err поток для stderr
 *  @return результат выполнения (exit_code и флаг выхода из оболочки)
 */
RunResult execute(const TokenList& args, std::ostream& out, std::ostream& err);

}  // namespace cli

#endif  // FLUFFY_TRIBBLE_EXECUTOR_HPP
