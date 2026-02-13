#ifndef fluffy_tribble_EXECUTION_CONTEXT_HPP
#define fluffy_tribble_EXECUTION_CONTEXT_HPP

#include <string>
#include <unordered_map>

namespace fluffy_tribble {

/**
 * Глобальное состояние интерпретатора: переменные окружения,
 * текущая директория, флаги завершения и код возврата.
 */
class ExecutionContext {
public:
    /** Отображение имя переменной окружения → значение. */
    using EnvMap = std::unordered_map<std::string, std::string>;

    /**
     * Создаёт контекст и инициализирует окружение из процесса (environ).
     */
    ExecutionContext();

    /**
     * Возвращает изменяемую ссылку на переменные окружения.
     */
    EnvMap &env();

    /**
     * Возвращает константную ссылку на переменные окружения.
     */
    const EnvMap &env() const;

    /**
     * Устанавливает переменную окружения name в value.
     */
    void set_env(const std::string &name, const std::string &value);

    /**
     * Возвращает текущую рабочую директорию.
     */
    std::string cwd() const;

    /**
     * Устанавливает текущую рабочую директорию (для согласованности с shell).
     */
    void set_cwd(const std::string &path);

    /**
     * Флаг выхода: true после вызова команды exit.
     */
    bool is_exit() const;

    /**
     * Устанавливает флаг выхода (вызывается командой exit).
     */
    void set_exit(bool value);

    /**
     * Код возврата последней команды (для встроенных — по усмотрению, для
     * внешних — их код).
     */
    int last_status() const;

    /**
     * Устанавливает код возврата последней команды.
     */
    void set_last_status(int status);

    /**
     * Целевой код возврата при выходе (аргумент exit).
     */
    int exit_code() const;

    /**
     * Устанавливает целевой код возврата при выходе.
     */
    void set_exit_code(int code);

private:
    EnvMap env_;
    std::string cwd_;
    bool is_exit_ = false;
    int last_status_ = 0;
    int exit_code_ = 0;
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_EXECUTION_CONTEXT_HPP
