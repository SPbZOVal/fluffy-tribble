#include "builtins.hpp"
#include <fstream>
#include <sstream>

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
void run<CommandID::CAT>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &
) {
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

template <>
void run<CommandID::ECHO>(
    const std::vector<std::string> &args,
    ReaderT &,
    WriterT &output,
    WriterT &,
    ExecutionContext &
) {
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i != 0) {
            output << ' ';
        }
        output << args[i];
    }
    output << '\n';
}

template <>
void run<CommandID::WC>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &
) {
    ReaderT *in = &input;
    std::ifstream file;
    if (!args.empty()) {
        file.open(args[0]);
        if (!file) {
            err << "wc: cannot open '" << args[0] << "'\n";
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
        bytes += line.size() + 1;  // + newline
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
}

template <>
void run<CommandID::PWD>(
    const std::vector<std::string> &,
    ReaderT &,
    WriterT &output,
    WriterT &,
    ExecutionContext &ctx
) {
    output << ctx.cwd() << '\n';
}

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
}

}  // namespace fluffy_tribble
