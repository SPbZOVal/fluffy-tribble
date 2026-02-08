/** @file tokenizer.cpp
 *  Реализация разбора строки команды в аргументы.
 */

#include "tokenizer.hpp"
#include <cctype>

namespace cli {

namespace {

/** Проверяет, является ли символ открывающей или закрывающей кавычкой. */
bool isQuote(char c) {
  return c == '\'' || c == '"';
}

}  // namespace

TokenList tokenize(const std::string& line) {
  TokenList result;
  std::string current;
  const std::size_t n = line.size();
  std::size_t i = 0;

  while (i < n) {
    while (i < n && std::isspace(static_cast<unsigned char>(line[i])))
      ++i;
    if (i >= n)
      break;

    if (isQuote(line[i])) {
      const char quote = line[i];
      ++i;
      while (i < n && line[i] != quote) {
        current += line[i];
        ++i;
      }
      if (i < n)
        ++i;  // skip closing quote
      result.push_back(std::move(current));
      current.clear();
      continue;
    }

    while (i < n && !std::isspace(static_cast<unsigned char>(line[i])) && !isQuote(line[i])) {
      current += line[i];
      ++i;
    }
    if (!current.empty()) {
      result.push_back(std::move(current));
      current.clear();
    }
  }

  return result;
}

}  // namespace cli
