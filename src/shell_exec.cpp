#include "shell_exec.hpp"

#include <cstdlib>    
#include <cstring>    
#include <fcntl.h>
#include <iostream>   
#include <string>     
#include <sys/wait.h> 
#include <unistd.h>   
#include <vector>     

void execute_command(const std::vector<std::string> &args,
                     const std::string &path) {
    pid_t pid = fork();
    if (pid == 0) {
        int stdout_fd = -1, stderr_fd = -1;
        std::vector<char *> exec_args;

        for (size_t i = 0; i < args.size(); ++i) {
            if ((args[i] == ">" || args[i] == "1>") && i + 1 < args.size()) {
                stdout_fd = open(args[i + 1].c_str(),
                                 O_CREAT | O_WRONLY | O_TRUNC, 0644);
                ++i;
            } else if ((args[i] == ">>" || args[i] == "1>>") &&
                       i + 1 < args.size()) {
                stdout_fd = open(args[i + 1].c_str(),
                                 O_CREAT | O_WRONLY | O_APPEND, 0644);
                ++i;
            } else if ((args[i] == "2>" && i + 1 < args.size())) {
                stderr_fd = open(args[i + 1].c_str(),
                                 O_CREAT | O_WRONLY | O_TRUNC, 0644);
                ++i;
            } else if ((args[i] == "2>>" && i + 1 < args.size())) {
                stderr_fd = open(args[i + 1].c_str(),
                                 O_CREAT | O_WRONLY | O_APPEND, 0644);
                ++i;
            } else {
                exec_args.push_back(const_cast<char *>(args[i].c_str()));
            }
        }

        if (stdout_fd >= 0) {
            dup2(stdout_fd, STDOUT_FILENO);
            close(stdout_fd);
        }

        if (stderr_fd >= 0) {
            dup2(stderr_fd, STDERR_FILENO);
            close(stderr_fd);
        }

        exec_args.push_back(nullptr);
        execv(path.c_str(), exec_args.data());
        perror("execv");
        std::exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
    }
}
