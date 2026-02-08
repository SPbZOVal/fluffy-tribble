#include "execution_context.hpp"
#include <unistd.h>
#include <cerrno>
#include <cstring>

#if !defined(_WIN32) && !defined(_WIN64)
extern char **environ;
#endif

namespace fluffy_tribble {

namespace {

std::string get_cwd() {
    std::string path;
    path.resize(256);
    while (true) {
        if (::getcwd(path.data(), path.size()) != nullptr) {
            path.resize(std::char_traits<char>::length(path.c_str()));
            return path;
        }
        if (errno != ERANGE) {
            return "";
        }
        path.resize(path.size() * 2);
    }
}

void load_environ(ExecutionContext::EnvMap &out) {
#if !defined(_WIN32) && !defined(_WIN64)
    if (!environ) {
        return;
    }
    for (char **p = environ; *p; ++p) {
        std::string s(*p);
        auto eq = s.find('=');
        if (eq != std::string::npos) {
            out[s.substr(0, eq)] = s.substr(eq + 1);
        }
    }
#endif
}

}  // namespace

ExecutionContext::ExecutionContext() : cwd_(get_cwd()) {
    load_environ(env_);
}

ExecutionContext::EnvMap &ExecutionContext::env() {
    return env_;
}

const ExecutionContext::EnvMap &ExecutionContext::env() const {
    return env_;
}

void ExecutionContext::set_env(
    const std::string &name,
    const std::string &value
) {
    env_[name] = value;
}

std::string ExecutionContext::cwd() const {
    return cwd_;
}

void ExecutionContext::set_cwd(const std::string &path) {
    cwd_ = path;
}

bool ExecutionContext::is_exit() const {
    return is_exit_;
}

void ExecutionContext::set_exit(bool value) {
    is_exit_ = value;
}

int ExecutionContext::last_status() const {
    return last_status_;
}

void ExecutionContext::set_last_status(int status) {
    last_status_ = status;
}

int ExecutionContext::exit_code() const {
    return exit_code_;
}

void ExecutionContext::set_exit_code(int code) {
    exit_code_ = code;
}

}  // namespace fluffy_tribble
