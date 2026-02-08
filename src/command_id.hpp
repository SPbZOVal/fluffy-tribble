#ifndef LKA_COMMAND_ID_HPP
#define LKA_COMMAND_ID_HPP

namespace lka {

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

}  // namespace lka

#endif  // LKA_COMMAND_ID_HPP
