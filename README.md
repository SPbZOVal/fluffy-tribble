# fluffy-tribble

[![CI](https://github.com/SPbZOVal/fluffy-tribble/actions/workflows/ci.yml/badge.svg?branch=release)](https://github.com/SPbZOVal/fluffy-tribble/actions/workflows/ci.yml)
[![PR title](https://github.com/SPbZOVal/fluffy-tribble/actions/workflows/pr-title.yml/badge.svg?branch=release)](https://github.com/SPbZOVal/fluffy-tribble/actions/workflows/pr-title.yml)

CLI task — интерпретатор командной строки с REPL. Первая часть: встроенные команды (cat, echo, wc, pwd, exit), внешние программы, кавычки; без подстановок и пайпов.

### Team
Grebenkin Ivan, Dorosev Anton

### Build

Сборка из консоли (Windows и Linux):

```bash
git clone git@github.com:SPbZOVal/fluffy-tribble.git
cd fluffy-tribble
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

На Windows для генерации проектов Visual Studio можно указать: `cmake .. -DCMAKE_BUILD_TYPE=Release -A x64`.

### Run

После сборки исполняемый файл интерпретатора:

- **Linux / macOS:** `./build/cli` или `build/cli`
- **Windows:** `build\Release\cli.exe` (или `build\cli.exe` в зависимости от генератора)

Запуск в интерактивном режиме (REPL): приглашение `>>>`, ввод команды, вывод результата.

### Tests

Юнит-тесты (Catch2) собираются в исполняемый файл `tests` и запускаются через CTest:

```bash
cd build
ctest -C Release --output-on-failure
```

Или напрямую: `./build/tests` (Linux/macOS), `build\Release\tests.exe` (Windows).

### Architecture

Описание архитектуры CLI и REPL: [arch.md](arch.md).

### Documentation

[documentation](https://github.com/SPbZOVal/fluffy-tribble/tree/main/docs).

## Examples

```sh
echo "Hello, world!"
>>>Hello, world!
```

```sh
FILE=example.txt
cat $FILE
>>>Some example text
```

```sh
cat example.txt | wc
>>>1 3 18
```

```sh
echo 123 | wc
>>>1 1 3
```

```sh
x=ex y=it
$x$y
>>>exit
```

## Expected behavior

### Built-in commands

- `cat <file>`
  Prints the file contents to stdout.

- `echo <args...>`
  Prints all arguments to stdout (separated by spaces).

- `wc <file>`
  Prints three numbers: lines, words, bytes.

- `pwd`
  Prints the current working directory.

- `exit`
  Terminates the interpreter.

### Quoting rules

- Single quotes '...' (full quoting):
  - Everything inside is treated literally.
  - Variable expansion ($NAME) must not be performed.
  - Spaces inside stay part of the same argument.

- Double quotes "..." (weak quoting):
  - Spaces inside stay part of the same argument.
  - Variable expansion ($NAME) must be performed.

### Environment and variable expansion

- Assignments of the form NAME=value set/update variables in the interpreter environment.
- $NAME expands to the current value of NAME.
- Concatenated expansions must work, e.g.:
  x=ex y=it
  $x$y
  produces:
  exit

### External commands

If the command name is not one of the built-ins, the interpreter should attempt to execute it as an external program.

### Pipelines

A pipeline cmd1 | cmd2 | ... | cmdN must:
- connect stdout of each command to stdin of the next command
- work for both built-ins and external commands
- preserve the usual left-to-right data flow semantics