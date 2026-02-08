#include "pipe_executor.hpp"
#include "command_executor.hpp"

namespace lka {

void PipeExecutor::execute(const Pipe& pipe,
                           std::istream& input,
                           std::ostream& output,
                           std::ostream& error,
                           ExecutionContext& ctx) {
    for (const ParsedCommand& cmd : pipe) {
        if (ctx.is_exit()) break;
        CommandExecutor::execute(cmd, input, output, error, ctx);
    }
}

}  // namespace lka
