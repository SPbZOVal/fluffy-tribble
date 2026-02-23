#include <argparse/argparse.hpp>
#include <fstream>
#include <sstream>
#include "builtins.hpp"

namespace fluffy_tribble {
template <>
void run<CommandID::WC>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
) {
    ReaderT *in = &input;
    std::ifstream file;
    if (!args.empty()) {
        file.open(args[0]);
        if (!file) {
            err << "wc: cannot open '" << args[0] << "'\n";
            ctx.set_last_status(1);
            return;
        }
        in = &file;
    }
    std::size_t lines = 0;
    std::size_t words = 0;
    std::size_t bytes = 0;
    std::string line;
    while (std::getline(*in, line)) {
        ++lines;
        bytes += line.size() + 1;
        std::istringstream iss(line);
        std::string w;
        while (iss >> w) {
            ++words;
        }
    }
    output << lines << ' ' << words << ' ' << bytes;
    if (!args.empty()) {
        output << ' ' << args[0];
    }
    output << '\n';
    ctx.set_last_status(0);
}
}  // namespace fluffy_tribble
