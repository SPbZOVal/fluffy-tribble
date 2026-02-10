#include "external_runner.hpp"
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

#ifdef __linux__
#include <sys/wait.h>
#endif

namespace fluffy_tribble {

namespace {

std::string find_in_path(const std::string &name, const ExecutionContext &ctx) {
    if (name.find('/') != std::string::npos) {
        return name;
    }
    auto it = ctx.env().find("PATH");
    if (it == ctx.env().end()) {
        return name;
    }
    const std::string &path_str = it->second;
    std::string dir;
    for (char c : path_str) {
        if (c == ':') {
            if (dir.empty()) {
                continue;
            }
            std::string candidate = dir + "/" + name;
            if (access(candidate.c_str(), X_OK) == 0) {
                return candidate;
            }
            dir.clear();
        } else {
            dir += c;
        }
    }
    if (!dir.empty()) {
        std::string candidate = dir + "/" + name;
        if (access(candidate.c_str(), X_OK) == 0) {
            return candidate;
        }
    }
    return name;
}

std::vector<std::string> env_to_vector(const ExecutionContext::EnvMap &env) {
    std::vector<std::string> out;
    out.reserve(env.size());
    std::ranges::transform(env, std::back_inserter(out), [](const auto &p) {
        return p.first + "=" + p.second;
    });
    return out;
}

std::vector<char *>
to_argv(const std::string &path, const std::vector<std::string> &args) {
    std::vector<char *> argv;
    argv.reserve(args.size() + 2);
    argv.push_back(const_cast<char *>(path.c_str()));
    std::ranges::transform(args, std::back_inserter(argv), [](const auto &a) {
        return const_cast<char *>(a.c_str());
    });
    argv.push_back(nullptr);
    return argv;
}

std::vector<char *> to_envp(const std::vector<std::string> &env_vec) {
    std::vector<char *> envp;
    envp.reserve(env_vec.size() + 1);
    std::ranges::transform(
        env_vec, std::back_inserter(envp),
        [](const auto &e) { return const_cast<char *>(e.c_str()); }
    );
    envp.push_back(nullptr);
    return envp;
}

}  // namespace

int ExternalRunner::run(
    const std::string &name,
    const std::vector<std::string> &args,
    ExecutionContext &ctx
) {
    std::string path = find_in_path(name, ctx);
    std::vector<std::string> env_vec = env_to_vector(ctx.env());
    std::vector<char *> argv = to_argv(path, args);
    std::vector<char *> envp = to_envp(env_vec);

    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        execve(path.c_str(), argv.data(), envp.data());
        _exit(127);
    }
    int status = 0;
    if (waitpid(pid, &status, 0) != pid) {
        return -1;
    }
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }
    return -1;
}

}  // namespace fluffy_tribble
