#include <iostream>
#include <stdexcept>
#include <string>
#include "command_executor.hpp"
#include "command_parser.hpp"
#include "execution_context.hpp"
#include "lexer.hpp"
#include "pipe_executor.hpp"

int main() {
    fluffy_tribble::ExecutionContext ctx;

    while (true) {
        std::cout << "$ " << std::flush;
        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }

        fluffy_tribble::Lexer lexer;
        fluffy_tribble::TokenStream tokens;
        try {
            tokens = lexer.tokenize(line, ctx);
        } catch (const std::runtime_error &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            continue;
        }

        fluffy_tribble::CommandParser parser;
        fluffy_tribble::Pipe pipe = parser.parse(tokens);

        if (pipe.empty()) {
            continue;
        }

        fluffy_tribble::PipeExecutor::execute(
            pipe, std::cin, std::cout, std::cerr, ctx
        );

        if (ctx.is_exit()) {
            return ctx.exit_code();
        }
    }

    return 0;
}