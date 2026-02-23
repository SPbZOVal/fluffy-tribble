#include <istream>
#include "builtins.hpp"

namespace fluffy_tribble {

template <>
void run<CommandID::PWD>(
    const std::vector<std::string> &,
    ReaderT &,
    WriterT &output,
    WriterT &,
    ExecutionContext &ctx
) {
    output << ctx.cwd() << '\n';
    ctx.set_last_status(0);
}
}  // namespace fluffy_tribble
