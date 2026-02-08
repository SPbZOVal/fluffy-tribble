#ifndef fluffy_tribble_COMMAND_MANAGER_HPP
#define fluffy_tribble_COMMAND_MANAGER_HPP

#include <iosfwd>
#include <string>
#include <vector>
#include "command_id.hpp"

namespace fluffy_tribble {

class ExecutionContext;

/**
 * Связывает строковое имя команды с тегом (CommandID).
 * Позволяет получать тег по имени, выполнять команду по тегу,
 * регистрировать команды во время выполнения.
 */
class CommandManager {
public:
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
     * Выполняет встроенную команду или exit по тегу.
     * Для ASSIGN и EXTERNAL не вызывается (обрабатываются исполнителем).
     * @param id Тег команды (CAT, ECHO, WC, PWD, EXIT).
     * @param args Аргументы команды.
     * @param input Входной поток.
     * @param output Выходной поток.
     * @param err Поток ошибок.
     * @param ctx Контекст выполнения.
     */
    static void execute(
        CommandID id,
        const std::vector<std::string> &args,
        std::istream &input,
        std::ostream &output,
        std::ostream &err,
        ExecutionContext &ctx
    );
};

}  // namespace fluffy_tribble

#endif  // fluffy_tribble_COMMAND_MANAGER_HPP
