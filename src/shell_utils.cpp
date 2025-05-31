#include "shell_utils.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

std::string find_executable(const std::string &command) {
    const char *path_env = std::getenv("PATH");
    if (!path_env)
        return "";

    std::istringstream iss(path_env);
    std::string dir;
    while (std::getline(iss, dir, ':')) {
        std::filesystem::path full_path = std::filesystem::path(dir) /
        command; if (std::filesystem::exists(full_path) &&
            access(full_path.c_str(), X_OK) == 0)
            return full_path.string();
    }
    return "";
}

bool run_pwd() {
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd))) {
        std::cout << cwd << std::endl;
        return true;
    } else {
        perror("pwd");
        return false;
    }
}

bool run_cd(const std::string &path) {
    if (path.empty()) {
        std::cerr << "cd: " << path << ": No such file or directory"
                  << std::endl;
        return false;
    }

    std::string clean_path = path;
    if (clean_path[0] == '~') {
        const char *home = std::getenv("HOME");
        if (!home) {
            std::cerr << "cd: HOME not set" << std::endl;
            return false;
        }
        clean_path = (clean_path == "~")
                         ? home
                         : std::string(home) + clean_path.substr(1);
    }

    std::filesystem::path abs_path = std::filesystem::absolute(clean_path);
    if (!std::filesystem::exists(abs_path) ||
        !std::filesystem::is_directory(abs_path)) {
        std::cerr << "cd: " << path << ": No such file or directory"
                  << std::endl;
        return false;
    }

    return chdir(abs_path.c_str()) == 0;
}
