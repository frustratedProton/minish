
# Minimal Shell (C++)

A small Unix-like shell implemented in C++.

## Features

- Built-in commands: `cd`, `pwd`, `echo`, `exit`, `type`, `history`
- Tab completion for built-ins and executables in `$PATH`
- Pipelining with `|`
- Output and error redirection: `>`, `>>`, `2>`, `2>>`
- Command history (via GNU Readline)
- Quote handling and escape sequences

## Building

```bash
mkdir -p build
cd build
cmake ..
make
````

## Running

```bash
./local.sh
```

## Dependencies

* C++17 or later
* GNU Readline
* CMake
