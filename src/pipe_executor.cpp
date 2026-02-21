#include "pipe_executor.hpp"
#include <sstream>
#include "command_executor.hpp"

namespace fluffy_tribble {

void PipeExecutor::execute(
    const Pipe &pipe,
    std::istream &input,
    std::ostream &output,
    std::ostream &error,
    ExecutionContext &ctx
) {
    if (pipe.empty()) {
        return;
    }

    if (pipe.size() == 1) {
        CommandExecutor::execute(pipe[0], input, output, error, ctx);
        return;
    }

    std::istringstream current_input;
    std::ostringstream current_output;

    for (std::size_t i = 0; i < pipe.size(); ++i) {
        if (ctx.is_exit()) {
            break;
        }

        const ParsedCommand &cmd = pipe[i];
        auto &in = i == 0 ? input : current_input;
        auto &out = i == pipe.size() - 1 ? output : current_output;
        CommandExecutor::execute(cmd, in, out, error, ctx);

        if (i < pipe.size() - 1) {
            current_input = std::istringstream(current_output.str());

            current_output.str("");
            current_output.clear();
        }
    }
}

}  // namespace fluffy_tribble
