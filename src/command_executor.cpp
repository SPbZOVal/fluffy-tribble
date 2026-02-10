#include "command_executor.hpp"
#include "command_manager.hpp"
#include "external_runner.hpp"
#include <iostream>

namespace fluffy_tribble {

void CommandExecutor::execute(
    const ParsedCommand &cmd,
    std::istream &input,
    std::ostream &output,
    std::ostream &error,
    ExecutionContext &ctx
) {

    if (cmd.id == CommandID::ASSIGN) {
        std::string value = cmd.args.empty() ? "" : cmd.args[0];
        ctx.set_env(cmd.name, value);
        ctx.set_last_status(0);
        return;
    }

    if (cmd.id == CommandID::EXTERNAL) {
        std::string name = cmd.args.empty() ? cmd.name : cmd.args[0];
        std::vector<std::string> args;
        if (cmd.args.size() > 1) {
            args.assign(cmd.args.begin() + 1, cmd.args.end());
        }
        int status = ExternalRunner::run(name, args, ctx);
        ctx.set_last_status(status);
    }

    CommandManager::CommandFn fn = CommandManager::get_command_fn(cmd.id);
    if (fn != nullptr) {
        fn(cmd.args, input, output, error, ctx);
    } else {
        error << "sh: " << cmd.name << ": command not found" << std::endl;
        ctx.set_last_status(127);
        return;
    }

    if (!ctx.is_exit()) {
        ctx.set_last_status(0);
    }
}

}  // namespace fluffy_tribble
