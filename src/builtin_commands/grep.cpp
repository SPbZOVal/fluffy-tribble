#include <argparse/argparse.hpp>
#include "builtins.hpp"

namespace fluffy_tribble {

template <>
void run<CommandID::GREP>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
) {
}
}  // namespace fluffy_tribble