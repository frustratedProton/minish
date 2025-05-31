#include "shell.hpp"
#include "builtin.hpp"
#include "completion.hpp"
#include "exec_pipes.hpp"
#include "shell_exec.hpp"
#include "shell_utils.hpp"
#include "tokenizer.hpp"
#include <cstdlib>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

int start_shell() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    rl_attempted_completion_function = custom_completion;

    char *buf;
    std::string cmd;

    while ((buf = readline("$ ")) != nullptr) {
        cmd = std::string(buf);
        free(buf);

        if (cmd.empty())
            continue;

        add_history(cmd.c_str());

        std::vector<std::string> pipeline_parts = split_pipeline(cmd);

        if (pipeline_parts.size() >= 2) {
            size_t n = pipeline_parts.size();
            std::vector<int> pipes(2 * (n - 1)); // each pipe has 2 fds

            for (size_t i = 0; i < n - 1; ++i) {
                if (pipe(&pipes[2 * i]) < 0) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            std::vector<pid_t> pids;

            for (size_t i = 0; i < n; ++i) {
                pid_t pid = fork();
                if (pid < 0) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Setup input pipe
                    if (i > 0) {
                        dup2(pipes[2 * (i - 1)], STDIN_FILENO);
                    }

                    // Setup output pipe
                    if (i < n - 1) {
                        dup2(pipes[2 * i + 1], STDOUT_FILENO);
                    }

                    // Close all pipes in child
                    for (size_t j = 0; j < 2 * (n - 1); ++j) {
                        close(pipes[j]);
                    }

                    std::string part = pipeline_parts[i];
                    if (!handle_builtin(part)) {
                        std::vector<std::string> args = split(part);
                        if (args.empty())
                            exit(1);
                        std::string path = find_executable(args[0]);
                        if (path.empty()) {
                            std::cerr << args[0] << ": command not found"
                                      << std::endl;
                            exit(1);
                        }

                        std::vector<char *> c_args;
                        for (auto &arg : args) {
                            c_args.push_back(const_cast<char *>(arg.c_str()));
                        }
                        c_args.push_back(nullptr);
                        execv(path.c_str(), c_args.data());
                        perror("execv");
                        exit(1);
                    }
                    exit(0);
                }
                pids.push_back(pid);
            }

            for (size_t i = 0; i < 2 * (n - 1); ++i) {
                close(pipes[i]);
            }

            for (pid_t pid : pids) {
                waitpid(pid, nullptr, 0);
            }
            continue;
        }

        if (handle_builtin(cmd)) {
            continue;
        }

        std::vector<std::string> args = split(cmd);
        if (args.empty())
            continue;

        std::string path = find_executable(args[0]);
        if (path.empty()) {
            std::cerr << args[0] << ": command not found" << std::endl;
            continue;
        }

        execute_command(args, path);
    }

    std::cout << std::endl;
    return EXIT_SUCCESS;
}