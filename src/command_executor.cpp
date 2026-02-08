#include "command_executor.hpp"
#include "builtins.hpp"
#include "command_id.hpp"
#include "external_runner.hpp"
#include <cctype>

namespace lka {

namespace {

bool name_eq(const std::string& a, const char* b) {
    if (a.size() != std::char_traits<char>::length(b)) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i])))
            return false;
    }
    return true;
}

void run_builtin_by_name(const std::string& name,
                        const std::vector<std::string>& args,
                        std::istream& input,
                        std::ostream& output,
                        std::ostream& error,
                        ExecutionContext& ctx) {
    if (name_eq(name, "cat")) {
        run_cat(args, input, output, error, ctx);
        return;
    }
    if (name_eq(name, "echo")) {
        run_echo(args, input, output, error, ctx);
        return;
    }
    if (name_eq(name, "wc")) {
        run_wc(args, input, output, error, ctx);
        return;
    }
    if (name_eq(name, "pwd")) {
        run_pwd(args, input, output, error, ctx);
        return;
    }
    if (name_eq(name, "exit")) {
        run_exit(args, input, output, error, ctx);
        return;
    }
    // fallback external
    int status = ExternalRunner::run(name, args, ctx);
    ctx.set_last_status(status);
}

}  // namespace

void CommandExecutor::execute(const ParsedCommand& cmd,
                              std::istream& input,
                              std::ostream& output,
                              std::ostream& error,
                              ExecutionContext& ctx) {
    switch (cmd.id) {
    case CommandID::ASSIGN: {
        std::string value = cmd.args.empty() ? "" : cmd.args[0];
        ctx.set_env(cmd.name, value);
        ctx.set_last_status(0);
        break;
    }
    case CommandID::EXIT:
        run_exit(cmd.args, input, output, error, ctx);
        break;
    case CommandID::BUILTIN:
        run_builtin_by_name(cmd.name, cmd.args, input, output, error, ctx);
        if (!ctx.is_exit()) {
            ctx.set_last_status(0);
        }
        break;
    case CommandID::EXTERNAL: {
        int status = ExternalRunner::run(cmd.name, cmd.args, ctx);
        ctx.set_last_status(status);
        break;
    }
    }
}

}  // namespace lka
