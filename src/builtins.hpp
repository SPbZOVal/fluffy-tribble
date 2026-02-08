/** @file builtins.hpp
 *  Встроенные команды интерпретатора: cat, echo, wc, pwd, exit.
 */

#ifndef FLUFFY_TRIBBLE_BUILTINS_HPP
#define FLUFFY_TRIBBLE_BUILTINS_HPP

#include <iosfwd>
#include <string>
#include <vector>

namespace cli {

/** Результат выполнения встроенной команды. */
struct RunResult {
  /** Код возврата (0 — успех). */
  int exit_code;
  /** true — интерпретатор должен завершиться (команда exit). */
  bool exit_shell;
};

/** Выводит содержимое файла в out, ошибки в err.
 *  @param args первый элемент игнорируется (имя команды), второй — путь к файлу
 *  @return RunResult{0, false} при успехе, {ненулевой код, false} при ошибке
 */
RunResult run_cat(const std::vector<std::string>& args,
                  std::ostream& out,
                  std::ostream& err);

/** Выводит аргументы в out через пробел (без первого — имени команды).
 *  @param args аргументы команды (args[0] — "echo", остальные — что печатать)
 *  @return RunResult{0, false}
 */
RunResult run_echo(const std::vector<std::string>& args,
                  std::ostream& out,
                  std::ostream& err);

/** Выводит количество строк, слов и байт файла в out.
 *  @param args args[1] — путь к файлу
 *  @return RunResult{0, false} при успехе, иначе ненулевой exit_code
 */
RunResult run_wc(const std::vector<std::string>& args,
                 std::ostream& out,
                 std::ostream& err);

/** Выводит текущую рабочую директорию в out.
 *  @return RunResult{0, false} при успехе
 */
RunResult run_pwd(const std::vector<std::string>& args,
                  std::ostream& out,
                  std::ostream& err);

/** Сигнализирует выход из интерпретатора (ничего не выводит).
 *  @return RunResult{0, true}
 */
RunResult run_exit(const std::vector<std::string>& args,
                   std::ostream& out,
                   std::ostream& err);

}  // namespace cli

#endif  // FLUFFY_TRIBBLE_BUILTINS_HPP
