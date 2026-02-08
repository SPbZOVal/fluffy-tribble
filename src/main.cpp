/** @file main.cpp
 *  Точка входа: REPL интерпретатора командной строки.
 */

#include "executor.hpp"
#include "tokenizer.hpp"
#include <iostream>
#include <string>

int main() {
  std::string line;
  while (std::cout << ">>> ", std::getline(std::cin, line)) {
    const cli::TokenList args = cli::tokenize(line);
    if (args.empty())
      continue;
    const cli::RunResult result = cli::execute(args, std::cout, std::cerr);
    if (result.exit_shell)
      return result.exit_code;
  }
  return 0;
}
