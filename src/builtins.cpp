#include "builtins.hpp"
#include "execution_context.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace lka {

namespace {

void cat_stream(std::istream& in, std::ostream& out) {
    std::string line;
    while (std::getline(in, line)) {
        out << line << '\n';
    }
}

}  // namespace

void run_cat(const std::vector<std::string>& args,
             std::istream& input,
             std::ostream& output,
             std::ostream& err,
             ExecutionContext&) {
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

void run_echo(const std::vector<std::string>& args,
              std::istream&,
              std::ostream& output,
              std::ostream&,
              ExecutionContext&) {
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i != 0) output << ' ';
        output << args[i];
    }
    output << '\n';
}

void run_wc(const std::vector<std::string>& args,
            std::istream& input,
            std::ostream& output,
            std::ostream& err,
            ExecutionContext&) {
    std::istream* in = &input;
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
        while (iss >> w) ++words;
    }
    output << lines << ' ' << words << ' ' << bytes;
    if (!args.empty()) {
        output << ' ' << args[0];
    }
    output << '\n';
}

void run_pwd(const std::vector<std::string>&,
             std::istream&,
             std::ostream& output,
             std::ostream&,
             ExecutionContext& ctx) {
    output << ctx.cwd() << '\n';
}

void run_exit(const std::vector<std::string>& args,
              std::istream&,
              std::ostream&,
              std::ostream&,
              ExecutionContext& ctx) {
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

}  // namespace lka
