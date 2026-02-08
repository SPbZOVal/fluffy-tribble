/** @file external_runner.cpp
 *  Реализация запуска внешней программы (Windows и Unix).
 */

#include "external_runner.hpp"
#include <algorithm>
#include <array>
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdlib>
#include <thread>
#else
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>

extern char** environ;
#endif

namespace cli {

#ifdef _WIN32

namespace {

/** Собирает командную строку в один строковый аргумент для CreateProcess.
 *  Экранирует кавычки в аргументах.
 */
std::string buildCommandLine(const std::vector<std::string>& args) {
  std::string cmd;
  for (std::size_t i = 0; i < args.size(); ++i) {
    if (i > 0)
      cmd += ' ';
    const std::string& a = args[i];
    bool need_quote = a.empty() || a.find(' ') != std::string::npos ||
                     a.find('"') != std::string::npos || a.find('\t') != std::string::npos;
    if (need_quote)
      cmd += '"';
    for (char c : a) {
      if (c == '"')
        cmd += "\\\"";
      else
        cmd += c;
    }
    if (need_quote)
      cmd += '"';
  }
  return cmd;
}

}  // namespace

int run_external(const std::vector<std::string>& args,
                 std::ostream& out,
                 std::ostream& err) {
  if (args.empty())
    return -1;

  const std::string cmdLine = buildCommandLine(args);

  SECURITY_ATTRIBUTES sa = {};
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = TRUE;

  HANDLE hOutRead = nullptr;
  HANDLE hOutWrite = nullptr;
  HANDLE hErrRead = nullptr;
  HANDLE hErrWrite = nullptr;

  if (!CreatePipe(&hOutRead, &hOutWrite, &sa, 0))
    return -1;
  if (!SetHandleInformation(hOutRead, HANDLE_FLAG_INHERIT, 0))
    return -1;

  if (!CreatePipe(&hErrRead, &hErrWrite, &sa, 0)) {
    CloseHandle(hOutRead);
    CloseHandle(hOutWrite);
    return -1;
  }
  if (!SetHandleInformation(hErrRead, HANDLE_FLAG_INHERIT, 0)) {
    CloseHandle(hOutRead);
    CloseHandle(hOutWrite);
    CloseHandle(hErrRead);
    CloseHandle(hErrWrite);
    return -1;
  }

  STARTUPINFOA si = {};
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  si.hStdOutput = hOutWrite;
  si.hStdError = hErrWrite;

  PROCESS_INFORMATION pi = {};
  std::vector<char> cmdLineCopy(cmdLine.begin(), cmdLine.end());
  cmdLineCopy.push_back('\0');

  const BOOL ok = CreateProcessA(
      nullptr,
      cmdLineCopy.data(),
      nullptr,
      nullptr,
      TRUE,
      CREATE_NO_WINDOW,
      nullptr,  // use current process environment
      nullptr,
      &si,
      &pi);

  CloseHandle(hOutWrite);
  CloseHandle(hErrWrite);
  hOutWrite = hErrWrite = nullptr;

  if (!ok) {
    err << "Failed to run: " << args[0] << "\n";
    CloseHandle(hOutRead);
    CloseHandle(hErrRead);
    return -1;
  }

  CloseHandle(pi.hThread);

  constexpr std::size_t BUF = 4096;
  std::array<char, BUF> buf;

  auto readOut = [&]() {
    DWORD n = 0;
    while (ReadFile(hOutRead, buf.data(), static_cast<DWORD>(buf.size()), &n, nullptr) && n > 0)
      out.write(buf.data(), static_cast<std::streamsize>(n));
  };
  auto readErr = [&]() {
    DWORD n = 0;
    while (ReadFile(hErrRead, buf.data(), static_cast<DWORD>(buf.size()), &n, nullptr) && n > 0)
      err.write(buf.data(), static_cast<std::streamsize>(n));
  };

  std::thread tOut(readOut);
  std::thread tErr(readErr);

  WaitForSingleObject(pi.hProcess, INFINITE);

  CloseHandle(hOutRead);
  CloseHandle(hErrRead);
  hOutRead = hErrRead = nullptr;

  tOut.join();
  tErr.join();

  DWORD exitCode = 0;
  GetExitCodeProcess(pi.hProcess, &exitCode);
  CloseHandle(pi.hProcess);

  return static_cast<int>(exitCode);
}

#else  // Unix

namespace {

/** Копирует вектор строк в массив char* для execve (последний элемент — nullptr). */
std::vector<char*> toArgv(const std::vector<std::string>& args) {
  std::vector<char*> argv;
  argv.reserve(args.size() + 1);
  for (const auto& a : args)
    argv.push_back(const_cast<char*>(a.c_str()));
  argv.push_back(nullptr);
  return argv;
}

}  // namespace

int run_external(const std::vector<std::string>& args,
                 std::ostream& out,
                 std::ostream& err) {
  if (args.empty())
    return -1;

  int outPipe[2];
  int errPipe[2];
  if (pipe(outPipe) != 0 || pipe(errPipe) != 0)
    return -1;

  const pid_t pid = fork();
  if (pid < 0) {
    close(outPipe[0]);
    close(outPipe[1]);
    close(errPipe[0]);
    close(errPipe[1]);
    return -1;
  }

  if (pid == 0) {
    close(outPipe[0]);
    close(errPipe[0]);
    dup2(outPipe[1], STDOUT_FILENO);
    dup2(errPipe[1], STDERR_FILENO);
    close(outPipe[1]);
    close(errPipe[1]);

    std::vector<char*> argv = toArgv(args);
    std::string program = args[0];
    if (program.find('/') == std::string::npos) {
      const char* path = std::getenv("PATH");
      if (path) {
        std::string pathStr(path);
        std::string candidate;
        for (std::size_t i = 0; i <= pathStr.size(); ++i) {
          if (i == pathStr.size() || pathStr[i] == ':') {
            if (!candidate.empty()) {
              candidate += '/';
              candidate += program;
              if (access(candidate.c_str(), X_OK) == 0) {
                program = candidate;
                argv[0] = const_cast<char*>(program.c_str());
                break;
              }
            }
            candidate.clear();
          } else
            candidate += pathStr[i];
        }
      }
    }
    execve(program.c_str(), argv.data(), environ);
    std::cerr << "Failed to execute: " << args[0] << "\n";
    _exit(127);
  }

  close(outPipe[1]);
  close(errPipe[1]);

  constexpr std::size_t BUF = 4096;
  std::array<char, BUF> buf;
  fd_set rd;
  int outFd = outPipe[0];
  int errFd = errPipe[0];
  const int maxFd = std::max(outFd, errFd) + 1;
  bool outOpen = true, errOpen = true;

  while (outOpen || errOpen) {
    FD_ZERO(&rd);
    if (outOpen)
      FD_SET(outFd, &rd);
    if (errOpen)
      FD_SET(errFd, &rd);
    int r = select(maxFd, &rd, nullptr, nullptr, nullptr);
    if (r <= 0)
      break;
    if (outOpen && FD_ISSET(outFd, &rd)) {
      ssize_t n = read(outFd, buf.data(), buf.size());
      if (n > 0)
        out.write(buf.data(), static_cast<std::streamsize>(n));
      else
        outOpen = false;
    }
    if (errOpen && FD_ISSET(errFd, &rd)) {
      ssize_t n = read(errFd, buf.data(), buf.size());
      if (n > 0)
        err.write(buf.data(), static_cast<std::streamsize>(n));
      else
        errOpen = false;
    }
  }

  close(outPipe[0]);
  close(errPipe[0]);

  int status = 0;
  while (waitpid(pid, &status, 0) < 0 && errno == EINTR)
    ;
  if (WIFEXITED(status))
    return WEXITSTATUS(status);
  return -1;
}

#endif

}  // namespace cli
