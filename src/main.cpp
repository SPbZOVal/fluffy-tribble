#include <iostream>
#include <string>
#include "command_executor.hpp"
#include "command_parser.hpp"
#include "execution_context.hpp"
#include "lexer.hpp"

int main() {
    fluffy_tribble::ExecutionContext ctx;

    while (true) {
        std::cout << "$ " << std::flush;
        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }

        fluffy_tribble::Lexer lexer;
        fluffy_tribble::TokenStream tokens = lexer.tokenize(line);

        fluffy_tribble::CommandParser parser;
        fluffy_tribble::Pipe pipe = parser.parse(tokens);

        if (pipe.empty()) {
            continue;
        }

        fluffy_tribble::CommandExecutor::execute(
            pipe[0], std::cin, std::cout, std::cerr, ctx
        );

        if (ctx.is_exit()) {
            return ctx.exit_code();
        }
    }

    return 0;
}
