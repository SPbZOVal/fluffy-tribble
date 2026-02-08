#ifndef fluffy_tribble_BUILTINS_HPP
#define fluffy_tribble_BUILTINS_HPP

#include <iosfwd>
#include <string>
#include <vector>

namespace fluffy_tribble {

class ExecutionContext;

/**
 * Реализации встроенных команд.
 * ReaderT = std::istream, WriterT = std::ostream (см. архитектуру).
 */

/**
 * cat <FILE> — выводит содержимое файла в output.
 * @param args args[0] — путь к файлу; при отсутствии — читает из input.
 * @param err Поток ошибок (сообщения об ошибках открытия файла).
 */
void run_cat(
    const std::vector<std::string> &args,
    std::istream &input,
    std::ostream &output,
    std::ostream &err,
    ExecutionContext &ctx
);

/**
 * echo — выводит аргументы в output через пробел, затем перевод строки.
 */
void run_echo(
    const std::vector<std::string> &args,
    std::istream &,
    std::ostream &output,
    std::ostream &err,
    ExecutionContext &ctx
);

/**
 * wc <FILE> — выводит количество строк, слов и байт в файле.
 * @param args args[0] — путь к файлу; при отсутствии — читает из input.
 */
void run_wc(
    const std::vector<std::string> &args,
    std::istream &input,
    std::ostream &output,
    std::ostream &err,
    ExecutionContext &ctx
);

/**
 * pwd — выводит текущую рабочую директорию из контекста.
 */
void run_pwd(
    const std::vector<std::string> &args,
    std::istream &,
    std::ostream &output,
    std::ostream &err,
    ExecutionContext &ctx
);

/**
 * exit [code] — устанавливает флаг выхода и код; пайплайн прерывается.
 */
void run_exit(
    const std::vector<std::string> &args,
    std::istream &,
    std::ostream &output,
    std::ostream &err,
    ExecutionContext &ctx
);

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_BUILTINS_HPP
