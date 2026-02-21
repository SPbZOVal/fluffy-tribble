#include "external_runner.hpp"
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>
#include <system_error>
#include <thread>
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

int get_fd_from_stream(std::ios &stream) {
    if (&stream == &std::cin) {
        return STDIN_FILENO;
    }
    if (&stream == &std::cout) {
        return STDOUT_FILENO;
    }
    if (&stream == &std::cerr) {
        return STDERR_FILENO;
    }
    return -1;
}

void write_stream_to_fd(std::istream &in, int fd) {
    char buffer[4096];
    while (in.read(buffer, sizeof(buffer)) || in.gcount() > 0) {
        std::streamsize n = in.gcount();
        ssize_t written = 0;
        while (written < n) {
            ssize_t ret = write(fd, buffer + written, n - written);
            if (ret == -1) {
                return;
            }
            written += ret;
        }
    }
}

void read_fd_to_stream(int fd, std::ostream &out) {
    char buffer[4096];
    ssize_t n;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        out.write(buffer, n);
    }
}

}  // namespace

int ExternalRunner::run(
    const std::string &name,
    const std::vector<std::string> &args,
    std::istream &input,
    std::ostream &output,
    std::ostream &error,
    ExecutionContext &ctx
) {
    std::string path = find_in_path(name, ctx);

    if (access(path.c_str(), F_OK) != 0) {
        error << "fluffy-tribble: " << name << ": command not found" << '\n';
        return 127;
    }

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

    int input_fd = get_fd_from_stream(input);
    int output_fd = get_fd_from_stream(output);
    int error_fd = get_fd_from_stream(error);

    bool need_pipe_in = (input_fd == -1);
    bool need_pipe_out = (output_fd == -1);
    bool need_pipe_err = (error_fd == -1);

    int pipe_in[2] = {-1, -1};
    int pipe_out[2] = {-1, -1};
    int pipe_err[2] = {-1, -1};

    if (need_pipe_in && pipe(pipe_in) == -1) {
        return -1;
    }
    if (need_pipe_out && pipe(pipe_out) == -1) {
        if (need_pipe_in) {
            close(pipe_in[0]);
            close(pipe_in[1]);
        }
        return -1;
    }
    if (need_pipe_err && pipe(pipe_err) == -1) {
        if (need_pipe_in) {
            close(pipe_in[0]);
            close(pipe_in[1]);
        }
        if (need_pipe_out) {
            close(pipe_out[0]);
            close(pipe_out[1]);
        }
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        if (need_pipe_in) {
            close(pipe_in[0]);
            close(pipe_in[1]);
        }
        if (need_pipe_out) {
            close(pipe_out[0]);
            close(pipe_out[1]);
        }
        if (need_pipe_err) {
            close(pipe_err[0]);
            close(pipe_err[1]);
        }
        return -1;
    }

    if (pid == 0) {
        if (need_pipe_in) {
            dup2(pipe_in[0], STDIN_FILENO);
            close(pipe_in[0]);
            close(pipe_in[1]);
        } else if (input_fd >= 0) {
            dup2(input_fd, STDIN_FILENO);
        }

        if (need_pipe_out) {
            dup2(pipe_out[1], STDOUT_FILENO);
            close(pipe_out[0]);
            close(pipe_out[1]);
        } else if (output_fd >= 0) {
            dup2(output_fd, STDOUT_FILENO);
        }

        if (need_pipe_err) {
            dup2(pipe_err[1], STDERR_FILENO);
            close(pipe_err[0]);
            close(pipe_err[1]);
        } else if (error_fd >= 0) {
            dup2(error_fd, STDERR_FILENO);
        }

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

    if (need_pipe_in) {
        close(pipe_in[0]);
    }
    if (need_pipe_out) {
        close(pipe_out[1]);
    }
    if (need_pipe_err) {
        close(pipe_err[1]);
    }

    std::thread writer;
    std::thread reader_out;
    std::thread reader_err;

    if (need_pipe_in) {
        writer = std::thread([&input, fd = pipe_in[1]]() {
            write_stream_to_fd(input, fd);
            close(fd);
        });
    }

    if (need_pipe_out) {
        reader_out = std::thread([&output, fd = pipe_out[0]]() {
            read_fd_to_stream(fd, output);
            close(fd);
        });
    }

    if (need_pipe_err) {
        reader_err = std::thread([&error, fd = pipe_err[0]]() {
            read_fd_to_stream(fd, error);
            close(fd);
        });
    }

    int status = 0;
    if (waitpid(pid, &status, 0) != pid) {
        status = -1;
    }

    if (writer.joinable()) {
        writer.join();
    }
    if (reader_out.joinable()) {
        reader_out.join();
    }
    if (reader_err.joinable()) {
        reader_err.join();
    }

    if (need_pipe_in) {
        close(pipe_in[1]);
    }
    if (need_pipe_out) {
        close(pipe_out[0]);
    }
    if (need_pipe_err) {
        close(pipe_err[0]);
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