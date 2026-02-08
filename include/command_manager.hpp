#ifndef fluffy_tribble_COMMAND_MANAGER_HPP
#define fluffy_tribble_COMMAND_MANAGER_HPP

#include <string>
#include <vector>
#include "command_id.hpp"

namespace fluffy_tribble {

/**
 * Связывает строковое имя команды с типом (CommandID).
 * Используется парсером и исполнителем для диспетчеризации.
 */
class CommandManager {
public:
    /**
     * Определяет тип команды по имени и аргументам.
     * @param name Имя команды.
     * @param args Аргументы (для присваивания уже разобраны парсером).
     * @return EXIT для "exit", BUILTIN для cat/echo/wc/pwd, иначе EXTERNAL.
     */
    static CommandID get_command_id(
        const std::string &name,
        const std::vector<std::string> &args
    );

    /**
     * Проверяет, является ли имя встроенной командой (cat, echo, wc, pwd).
     */
    static bool is_builtin(const std::string &name);
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_COMMAND_MANAGER_HPP
