#include "command_parser.hpp"
#include "execution_context.hpp"
#include "lexer.hpp"
#include "pipe_executor.hpp"
#include <iostream>
#include <string>

int main() {
    lka::ExecutionContext ctx;

    while (true) {
        std::cout << "$ " << std::flush;
        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }

        lka::Lexer lexer;
        lka::TokenStream tokens = lexer.tokenize(line);

        lka::CommandParser parser;
        lka::Pipe pipe = parser.parse(tokens);

        if (pipe.empty()) {
            continue;
        }

        lka::PipeExecutor::execute(pipe, std::cin, std::cout, std::cerr, ctx);

        if (ctx.is_exit()) {
            return ctx.exit_code();
        }
    }

    return 0;
}
