#include "builtin.hpp"
#include "shell_utils.hpp"
#include "tokenizer.hpp"

#include <fcntl.h>
#include <iostream>
#include <readline/history.h>
#include <string>
#include <unistd.h>
#include <vector>

bool handle_builtin(const std::string &input) {
    std::vector<std::string> args = split(input);
    if (args.empty())
        return false;

    if (args[0] == "echo") {
        std::vector<std::string> echo_args;
        std::string stdout_file, stderr_file;
        bool redirect_stdout = false, redirect_stderr = false,
             stdout_append = false, stderr_append = false;

        for (size_t i = 1; i < args.size(); ++i) {
            if ((args[i] == ">" || args[i] == "1>") && i + 1 < args.size()) {
                redirect_stdout = true;
                stdout_file = args[i + 1];
                stdout_append = false;
                ++i;
            } else if ((args[i] == ">>" || args[i] == "1>>") &&
                       i + 1 < args.size()) {
                redirect_stdout = true;
                stdout_file = args[i + 1];
                stdout_append = true;
                ++i;
            } else if (args[i] == "2>" && i + 1 < args.size()) {
                redirect_stderr = true;
                stderr_file = args[i + 1];
                stderr_append = false;
                ++i;
            } else if (args[i] == "2>>" && i + 1 < args.size()) {
                redirect_stderr = true;
                stderr_file = args[i + 1];
                stderr_append = true;
                ++i;
            } else {
                echo_args.push_back(args[i]);
            }
        }

        int saved_stdout = -1, saved_stderr = -1;
        int fd_out = -1, fd_err = -1;

        if (redirect_stdout) {
            fd_out =
                open(stdout_file.c_str(),
                     O_CREAT | O_WRONLY | (stdout_append ? O_APPEND : O_TRUNC),
                     0644);
            if (fd_out < 0) {
                perror("open stdout");
                return true;
            }
            saved_stdout = dup(STDOUT_FILENO);
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        if (redirect_stderr) {
            fd_err =
                open(stderr_file.c_str(),
                     O_CREAT | O_WRONLY | (stderr_append ? O_APPEND : O_TRUNC),
                     0644);
            if (fd_err < 0) {
                perror("open stderr");
                return true;
            }
            saved_stderr = dup(STDERR_FILENO);
            dup2(fd_err, STDERR_FILENO);
            close(fd_err);
        }

        // Actual echo output to stdout
        for (size_t i = 0; i < echo_args.size(); ++i) {
            std::cout << echo_args[i];
            if (i + 1 < echo_args.size())
                std::cout << " ";
        }
        std::cout << std::endl;

        if (redirect_stdout) {
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }

        if (redirect_stderr) {
            dup2(saved_stderr, STDERR_FILENO);
            close(saved_stderr);
        }

        return true;
    }

    if (input == "exit 0") {
        std::exit(0);
    }

    if (args[0] == "type" && args.size() == 2) {
        const std::string &arg = args[1];
        if (arg == "echo" || arg == "exit" || arg == "type" || arg == "pwd" ||
            arg == "cd" || arg == "history") {
            std::cout << arg << " is a shell builtin" << std::endl;
        } else {
            std::string path = find_executable(arg);
            if (!path.empty()) {
                std::cout << arg << " is " << path << std::endl;
            } else {
                std::cout << arg << ": not found" << std::endl;
            }
        }
        return true;
    }

    if (args[0] == "pwd") {
        return run_pwd();
    }

    if (args[0] == "cd") {
        if (args.size() == 1) {
            const char *home = std::getenv("HOME");
            if (!home) {
                std::cerr << "cd: HOME not set" << std::endl;
                return true;
            }
            run_cd(home);
        } else if (args.size() == 2) {
            run_cd(args[1]);
        } else {
            std::cerr << "cd: too many arguments" << std::endl;
        }
        return true;
    }

    if (args[0] == "history") {
        HIST_ENTRY **the_list = history_list();

        if (!the_list)
            return true;

        int total = 0;
        while (the_list[total])
            total++;

        int limit = total;

        if (args.size() == 2) {
            try {
                int n = std::stoi(args[1]);
                if (n >= 0 && n <= total) {
                    limit = n;
                }
            } catch (...) {
                // if stoi fail, print full hist.
            }
        }

        for (int i = total - limit; i < total; ++i) {
            std::cout << i + history_base << "  " << the_list[i]->line
                      << std::endl;
        }

        return true;
    }

    if (args[0] == "about") {
        std::cout << "Minish - A Simple Unix Shell\n"
                  << "Author: Aryan Kumar Singh\n"
                  << "Version: 1.0.1\n"
                  << "Project: minish\n"
                  << "License: MIT\n"
                  << "Description: This is a lightweight Unix shell supporting "
                     "basic built-in commands and redirection.\n";
        return true;
    }

    return false;
}
