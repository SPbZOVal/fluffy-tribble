#ifndef fluffy_tribble_BUILTINS_HPP
#define fluffy_tribble_BUILTINS_HPP

#include <iosfwd>
#include <string>
#include <vector>
#include "command_id.hpp"
#include "execution_context.hpp"

namespace fluffy_tribble {

/** Алиас для входного потока (архитектура). */
using ReaderT = std::istream;
/** Алиас для выходного потока (архитектура). */
using WriterT = std::ostream;

/**
 * Реализация команды по тегу CommandID.
 * Специализации: CAT, ECHO, WC, PWD, EXIT.
 * @param args Аргументы команды.
 * @param input Входной поток (для cat/wc при чтении из stdin).
 * @param output Выходной поток.
 * @param err Поток ошибок.
 * @param ctx Контекст выполнения.
 */
template <CommandID Id>
void run(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
);

/** Специализация: cat — выводит содержимое файла или stdin. */
template <>
void run<CommandID::CAT>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
);

/** Специализация: echo — выводит аргументы через пробел и перевод строки. */
template <>
void run<CommandID::ECHO>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
);

/** Специализация: wc — строки, слова, байты в файле или stdin. */
template <>
void run<CommandID::WC>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
);

/** Специализация: pwd — текущая рабочая директория. */
template <>
void run<CommandID::PWD>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
);

/** Специализация: grep — поиск в файле. */
template <>
void run<CommandID::GREP>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    [[maybe_unused]] ExecutionContext &ctx
);

/** Специализация: exit — устанавливает флаг выхода и код. */
template <>
void run<CommandID::EXIT>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
);

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_BUILTINS_HPP
