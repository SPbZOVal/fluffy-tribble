#ifndef fluffy_tribble_COMMAND_ID_HPP
#define fluffy_tribble_COMMAND_ID_HPP

namespace fluffy_tribble {

/**
 * Идентификатор типа команды: встроенная (по имени), присваивание,
 * exit или внешняя программа.
 */
enum class CommandID {
    /** Встроенная команда (cat, echo, wc, pwd). */
    BUILTIN,
    /** Присваивание переменной окружения (name=value). */
    ASSIGN,
    /** Команда выхода из интерпретатора. */
    EXIT,
    /** Внешняя программа (неизвестное имя). */
    EXTERNAL
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_COMMAND_ID_HPP
