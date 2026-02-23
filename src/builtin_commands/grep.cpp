#include <argparse/argparse.hpp>
#include <fstream>
#include <ostream>
#include <regex>
#include "builtins.hpp"

namespace fluffy_tribble {

namespace {
struct GrepArguments {
    std::string pattern;
    std::vector<std::string> files;
    int after_context;
    bool word_regexp;
    bool ignore_case;
};

void print_usage(WriterT &err) {
    err << "Usage: grep [OPTION]... PATTERNS [FILE]...\n";
}

GrepArguments parse_grep_arguments(
    const std::vector<std::string> &args,
    WriterT &output,
    WriterT &err
) {
    if (args.empty()) {
        print_usage(err);
        throw std::runtime_error("missing pattern");
    }

    GrepArguments grep_args;

    argparse::ArgumentParser program(
        "grep", "1.0", argparse::default_arguments::help, false, output
    );
    program.add_argument("pattern").help("Search pattern");
    program.add_argument("files").remaining().help("Files to search");
    program.add_argument("-w").default_value(false).implicit_value(true).help(
        "The expression is searched for as a word"
    );
    program.add_argument("-i").default_value(false).implicit_value(true).help(
        "Perform case insensitive matching.  By default, grep is case "
        "sensitive."
    );
    program.add_argument("-A").scan<'i', int>().default_value(0).help(
        "Print num lines of trailing context after each match."
    );

    try {
        program.parse_args(args);
    } catch (const std::exception &e) {
        err << "grep: " << e.what() << "\n";
        throw;
    }

    grep_args.pattern = program.get<std::string>("pattern");

    if (program.present("files")) {
        grep_args.files = program.get<std::vector<std::string>>("files");
    }

    grep_args.word_regexp = program.get<bool>("-w");
    grep_args.ignore_case = program.get<bool>("-i");
    grep_args.after_context = program.get<int>("-A");

    if (grep_args.pattern.empty()) {
        print_usage(err);
        throw std::runtime_error("empty pattern");
    }
    return grep_args;
}

std::regex prepare_regex(const GrepArguments &grep_args, WriterT &) {
    std::string pattern = grep_args.pattern;

    if (grep_args.word_regexp) {
        pattern = "\\b(" + pattern + ")\\b";
    }

    std::regex::flag_type flags = std::regex::ECMAScript;
    if (grep_args.ignore_case) {
        flags |= std::regex::icase;
    }

    return std::regex(pattern, flags);
}

void process_stream(
    ReaderT &input,
    WriterT &output,
    const GrepArguments &grep_args,
    const std::regex &re
) {
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(input, line)) {
        lines.push_back(line);
    }

    int n = static_cast<int>(lines.size());
    std::vector<bool> to_print(n, false);

    for (int i = 0; i < n; ++i) {
        if (std::regex_search(lines[i], re)) {
            int from = i;
            int to = std::min(n - 1, i + grep_args.after_context);
            for (int j = from; j <= to; ++j) {
                to_print[j] = true;
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        if (to_print[i]) {
            output << lines[i] << "\n";
        }
    }
}

}  // namespace

template <>
void run<CommandID::GREP>(
    const std::vector<std::string> &args,
    ReaderT &input,
    WriterT &output,
    WriterT &err,
    ExecutionContext &ctx
) {
    GrepArguments grep_args;
    try {
        grep_args = parse_grep_arguments(args, output, err);
    } catch (const std::exception &e) {
        ctx.set_last_status(2);
        return;
    }

    std::regex re;
    try {
        re = prepare_regex(grep_args, err);
    } catch (const std::regex_error &e) {
        err << "grep: invalid regex: " << e.what() << "\n";
        ctx.set_last_status(2);
        return;
    }

    bool any_error = false;
    if (grep_args.files.empty()) {
        process_stream(input, output, grep_args, re);
    } else {
        for (const auto &filename : grep_args.files) {
            std::ifstream file(filename);
            if (!file) {
                err << "grep: " << filename << ": No such file or directory\n";
                any_error = true;
                continue;
            }
            process_stream(file, output, grep_args, re);
        }
    }
    ctx.set_last_status(any_error ? 1 : 0);
}
}  // namespace fluffy_tribble
