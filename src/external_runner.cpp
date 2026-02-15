#include "external_runner.hpp"
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>
#include <system_error>
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

}  // namespace

int ExternalRunner::run(
    const std::string &name,
    const std::vector<std::string> &args,
    ExecutionContext &ctx
) {
    std::string path = find_in_path(name, ctx);
    std::vector<std::string> env_vec = env_to_vector(ctx.env());
    std::vector<std::string> argv_strings;

    if (!args.empty()) {
        argv_strings = args;
    } else {
        argv_strings.push_back(path);
    }

    std::vector<char *> argv;
    argv.reserve(argv_strings.size() + 1);
    for (auto &s : argv_strings) {
        argv.push_back(const_cast<char *>(s.c_str()));
    }
    argv.push_back(nullptr);

    std::vector<char *> envp;
    envp.reserve(env_vec.size() + 1);
    for (auto &s : env_vec) {
        envp.push_back(const_cast<char *>(s.c_str()));
    }
    envp.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        execve(path.c_str(), argv.data(), envp.data());

        int err = errno;
        std::error_code ec(err, std::system_category());
        std::cerr << ec.message() << '\n';

        if (err == ENOENT) {
            _exit(127);
        } else if (err == EACCES) {
            _exit(126);
        } else {
            _exit(1);
        }
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