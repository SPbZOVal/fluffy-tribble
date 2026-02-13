#ifndef fluffy_tribble_COMMAND_ID_HPP
#define fluffy_tribble_COMMAND_ID_HPP

namespace fluffy_tribble {

/**
 * Идентификатор типа команды: встроенные по имени, присваивание,
 * exit или внешняя программа.
 */
enum class CommandID {
    /** Встроенная команда cat. */
    CAT,
    /** Встроенная команда echo. */
    ECHO,
    /** Встроенная команда wc. */
    WC,
    /** Встроенная команда pwd. */
    PWD,
    /** Присваивание переменной окружения ($name=value). */
    ASSIGN,
    /** Команда выхода из интерпретатора. */
    EXIT,
    /** Внешняя программа (неизвестное имя). */
    EXTERNAL
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_COMMAND_ID_HPP
