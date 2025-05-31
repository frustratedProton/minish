#pragma once

#include <string>
#include <unordered_map>

extern std::unordered_map<std::string, std::string> user_env;

std::string expand_variables(const std::string &input);

bool process_variable_assignment(const std::string &input);