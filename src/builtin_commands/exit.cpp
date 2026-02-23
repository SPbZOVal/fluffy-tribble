#include "builtins.hpp"

namespace fluffy_tribble {

template <>
void run<CommandID::EXIT>(
    const std::vector<std::string> &args,
    ReaderT &,
    WriterT &,
    WriterT &,
    ExecutionContext &ctx
) {
    int code = 0;
    if (!args.empty()) {
        try {
            code = std::stoi(args[0]);
        } catch (...) {
            code = 0;
        }
    }
    ctx.set_exit_code(code);
    ctx.set_exit(true);
    ctx.set_last_status(code);
}
}  // namespace fluffy_tribble
