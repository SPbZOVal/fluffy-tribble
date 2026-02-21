#include <fstream>
#include "builtins.hpp"

namespace fluffy_tribble {

namespace {

void cat_stream(ReaderT &in, WriterT &out) {
    std::string line;
    while (std::getline(in, line)) {
        out << line << '\n';
    }
}

}  // namespace

template <>
void run<
    CommandID::
        CAT>(const std::vector<std::string> &args, ReaderT &input, WriterT &output, WriterT &err, ExecutionContext &) {
    if (args.empty()) {
        cat_stream(input, output);
        return;
    }
    std::ifstream f(args[0]);
    if (!f) {
        err << "cat: cannot open '" << args[0] << "'\n";
        return;
    }
    cat_stream(f, output);
}
}  // namespace fluffy_tribble
