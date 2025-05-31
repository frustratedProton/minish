#pragma once

#include <string>

std::string find_executable(const std::string &command);

bool run_pwd();

bool run_cd(const std::string &path);