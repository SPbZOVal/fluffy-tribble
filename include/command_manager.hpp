#ifndef fluffy_tribble_COMMAND_MANAGER_HPP
#define fluffy_tribble_COMMAND_MANAGER_HPP

#include <iosfwd>
#include <string>
#include <vector>
#include "command_id.hpp"

namespace fluffy_tribble {

class ExecutionContext;

class CommandManager {
public:
    /**
     * Тип указателя на реализацию команды (run<CommandID>).
     */
    using CommandFn = void (*)(
        const std::vector<std::string> &args,
        std::istream &input,
        std::ostream &output,
        std::ostream &err,
        ExecutionContext &ctx
    );

    /**
     * Возвращает тег команды по имени.
     * @param name Имя команды.
     * @return Зарегистрированный CommandID или EXTERNAL, если имя не найдено.
     */
    static CommandID get_command_id(const std::string &name);

    /**
     * Регистрирует имя команды для данного тега (runtime).
     * @param name Имя команды.
     * @param id Тег (CAT, ECHO, WC, PWD, EXIT и т.д.).
     */
    static void register_command(const std::string &name, CommandID id);

    /**
     * Возвращает указатель на реализацию команды по тегу.
     * Для ASSIGN и EXTERNAL возвращает nullptr.
     */
    static CommandFn get_command_fn(CommandID id);
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_COMMAND_MANAGER_HPP
