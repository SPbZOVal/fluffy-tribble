
#include <fstream>
#include "builtins.hpp"

namespace fluffy_tribble {
template <>
void run<CommandID::ECHO>(
    const std::vector<std::string> &args,
    ReaderT &,
    WriterT &output,
    WriterT &,
    ExecutionContext &ctx
) {
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i != 0) {
            output << ' ';
        }
        output << args[i];
    }
    output << '\n';
    ctx.set_last_status(0);
}
}  // namespace fluffy_tribble
