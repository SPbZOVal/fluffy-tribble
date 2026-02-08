/** @file builtins.cpp
 *  Реализация встроенных команд.
 */

#include "builtins.hpp"
#include <cctype>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

namespace cli {

namespace {

/** Подсчёт строк, слов и байт за один проход. */
void countLinesWordsBytes(std::istream& is,
                          std::size_t& lines,
                          std::size_t& words,
                          std::size_t& bytes) {
  lines = 0;
  words = 0;
  bytes = 0;
  bool in_word = false;
  char c;
  while (is.get(c)) {
    ++bytes;
    if (c == '\n')
      ++lines;
    const bool space = std::isspace(static_cast<unsigned char>(c));
    if (space)
      in_word = false;
    else if (!in_word) {
      in_word = true;
      ++words;
    }
  }
}

}  // namespace

RunResult run_cat(const std::vector<std::string>& args,
                 std::ostream& out,
                 std::ostream& err) {
  if (args.size() < 2) {
    err << "cat: missing file operand\n";
    return RunResult{1, false};
  }
  const std::string& path = args[1];
  std::ifstream f(path);
  if (!f) {
    err << "cat: cannot open '" << path << "'\n";
    return RunResult{1, false};
  }
  out << f.rdbuf();
  if (!f) {
    err << "cat: error reading '" << path << "'\n";
    return RunResult{1, false};
  }
  return RunResult{0, false};
}

RunResult run_echo(const std::vector<std::string>& args,
                  std::ostream& out,
                  std::ostream& /*err*/) {
  for (std::size_t i = 1; i < args.size(); ++i) {
    if (i > 1)
      out << ' ';
    out << args[i];
  }
  if (args.size() > 1)
    out << '\n';
  return RunResult{0, false};
}

RunResult run_wc(const std::vector<std::string>& args,
                 std::ostream& out,
                 std::ostream& err) {
  if (args.size() < 2) {
    err << "wc: missing file operand\n";
    return RunResult{1, false};
  }
  const std::string& path = args[1];
  std::ifstream f(path);
  if (!f) {
    err << "wc: cannot open '" << path << "'\n";
    return RunResult{1, false};
  }
  std::size_t lines = 0;
  std::size_t words = 0;
  std::size_t bytes = 0;
  countLinesWordsBytes(f, lines, words, bytes);
  out << lines << ' ' << words << ' ' << bytes << '\n';
  return RunResult{0, false};
}

RunResult run_pwd(const std::vector<std::string>& /*args*/,
                  std::ostream& out,
                  std::ostream& /*err*/) {
  char buf[4096];
  if (getcwd(buf, sizeof(buf))) {
    out << buf << '\n';
    return RunResult{0, false};
  }
  return RunResult{1, false};
}

RunResult run_exit(const std::vector<std::string>& /*args*/,
                   std::ostream& /*out*/,
                   std::ostream& /*err*/) {
  return RunResult{0, true};
}

}  // namespace cli
