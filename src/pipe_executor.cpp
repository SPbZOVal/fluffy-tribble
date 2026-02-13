#include "pipe_executor.hpp"
#include "command_executor.hpp"
#include <sstream>
#include <memory>

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

    std::unique_ptr<std::istringstream> current_input;
    std::ostringstream current_output;

    for (std::size_t i = 0; i < pipe.size(); ++i) {
        if (ctx.is_exit()) {
            break;
        }

        const ParsedCommand &cmd = pipe[i];
        std::istream *in = (i == 0) ? &input : current_input.get();
        std::ostream *out = (i == pipe.size() - 1) ? &output : &current_output;
        CommandExecutor::execute(cmd, *in, *out, error, ctx);

        if (i < pipe.size() - 1) {
            current_input = std::make_unique<std::istringstream>(current_output.str());
            
            current_output.str("");
            current_output.clear();
        }
    }
}

}  // namespace fluffy_tribble
