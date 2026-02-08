#include "command_executor.hpp"
#include "command_manager.hpp"
#include "external_runner.hpp"

namespace fluffy_tribble {

void CommandExecutor::execute(
    const ParsedCommand &cmd,
    std::istream &input,
    std::ostream &output,
    std::ostream &error,
    ExecutionContext &ctx
) {
    switch (cmd.id) {
        case CommandID::ASSIGN: {
            std::string value = cmd.args.empty() ? "" : cmd.args[0];
            ctx.set_env(cmd.name, value);
            ctx.set_last_status(0);
            break;
        }
        case CommandID::EXIT:
        case CommandID::CAT:
        case CommandID::ECHO:
        case CommandID::WC:
        case CommandID::PWD:
            CommandManager::execute(
                cmd.id, cmd.args, input, output, error, ctx
            );
            if (!ctx.is_exit()) {
                ctx.set_last_status(0);
            }
            break;
        case CommandID::EXTERNAL: {
            std::string name = cmd.args.empty() ? cmd.name : cmd.args[0];
            std::vector<std::string> args;
            if (cmd.args.size() > 1) {
                args.assign(cmd.args.begin() + 1, cmd.args.end());
            }
            int status = ExternalRunner::run(name, args, ctx);
            ctx.set_last_status(status);
            break;
        }
    }
}

}  // namespace fluffy_tribble
