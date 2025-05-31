cmake_minimum_required(VERSION 3.15)
project(minimal_shell)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(include)

add_executable(shell
    src/main.cpp
    src/shell.cpp
    src/builtin.cpp
    src/completion.cpp
    src/exec_pipes.cpp
    src/shell_exec.cpp
    src/shell_utils.cpp
    src/tokenizer.cpp
    src/command_substitute.cpp
)

target_link_libraries(shell PRIVATE readline)
