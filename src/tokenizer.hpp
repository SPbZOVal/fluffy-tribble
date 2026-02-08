/** @file tokenizer.hpp
 *  Разбор строки команды в список аргументов с учётом кавычек.
 */

#ifndef FLUFFY_TRIBBLE_TOKENIZER_HPP
#define FLUFFY_TRIBBLE_TOKENIZER_HPP

#include <string>
#include <vector>

namespace cli {

/** Результат разбора: список аргументов команды (имя команды — первый элемент). */
using TokenList = std::vector<std::string>;

/** Разбирает строку команды в список аргументов.
 *  Поддерживаются одинарные и двойные кавычки: всё внутри кавычек — один аргумент.
 *  Подстановки переменных не выполняются.
 *  @param line строка, введённая пользователем
 *  @return список аргументов (пустой при пустой или только-пробельной строке)
 */
TokenList tokenize(const std::string& line);

}  // namespace cli

#endif  // FLUFFY_TRIBBLE_TOKENIZER_HPP
