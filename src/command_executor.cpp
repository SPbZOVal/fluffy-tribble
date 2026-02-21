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
            if (cmd.args.size() == 1) {
                ctx.set_env(cmd.name, cmd.args[0]);
            }
            break;
        }
        case CommandID::EXTERNAL: {
            int status = ExternalRunner::run(
                cmd.name, cmd.args, input, output, error, ctx
            );
            ctx.set_last_status(status);
            break;
        }
        default: {
            auto fn = CommandManager::get_command_fn(cmd.id);
            if (fn) {
                fn(cmd.args, input, output, error, ctx);
                ctx.set_last_status(0);
            }

            if (!ctx.is_exit()) {
                ctx.set_last_status(0);
            }

            break;
        }
    }
}

}  // namespace fluffy_tribble