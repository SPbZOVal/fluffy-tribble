/** @file executor.cpp
 *  Реализация исполнителя команд.
 */

#include "executor.hpp"
#include "external_runner.hpp"
#include <string>

namespace cli {

RunResult execute(const TokenList& args, std::ostream& out, std::ostream& err) {
  if (args.empty())
    return RunResult{0, false};

  const std::string& cmd = args[0];

  if (cmd == "cat")
    return run_cat(args, out, err);
  if (cmd == "echo")
    return run_echo(args, out, err);
  if (cmd == "wc")
    return run_wc(args, out, err);
  if (cmd == "pwd")
    return run_pwd(args, out, err);
  if (cmd == "exit")
    return run_exit(args, out, err);

  const int code = run_external(args, out, err);
  return RunResult{code >= 0 ? code : 127, false};
}

}  // namespace cli
