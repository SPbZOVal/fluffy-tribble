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
     * @return Ссылка на отображение переменных окружения (имя -> значение).
     */
    EnvMap &env();

    /**
     * Возвращает константную ссылку на переменные окружения.
     * @return Константная ссылка на отображение переменных окружения (имя ->
     * значение).
     */
    const EnvMap &env() const;

    /**
     * Устанавливает переменную окружения name в value.
     * @param name Имя переменной окружения.
     * @param value Значение переменной окружения.
     */
    void set_env(const std::string &name, const std::string &value);

    /**
     * Возвращает текущую рабочую директорию.
     * @return Путь к текущей рабочей директории.
     */
    std::string cwd() const;

    /**
     * Устанавливает текущую рабочую директорию (для согласованности с shell).
     * @param path Путь к новой рабочей директории.
     */
    void set_cwd(const std::string &path);

    /**
     * Флаг выхода: true после вызова команды exit.
     * @return true если был вызван выход, false в противном случае.
     */
    bool is_exit() const;

    /**
     * Устанавливает флаг выхода (вызывается командой exit).
     * @param value Значение флага выхода.
     */
    void set_exit(bool value);

    /**
     * Код возврата последней команды (для встроенных — по усмотрению, для
     * внешних — их код).
     * @return Код возврата последней выполненной команды.
     */
    int last_status() const;

    /**
     * Устанавливает код возврата последней команды.
     * @param status Код возврата для последней команды.
     */
    void set_last_status(int status);

    /**
     * Целевой код возврата при выходе (аргумент exit).
     * @return Код возврата для выхода из программы.
     */
    int exit_code() const;

    /**
     * Устанавливает целевой код возврата при выходе.
     * @param code Код возврата для выхода из программы.
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
