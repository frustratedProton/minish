#include "var_expension.hpp"
#include <cstddef>
#include <string>

std::unordered_map<std::string, std::string> user_env;

std::string expand_variables(const std::string &input) {
    std::string result;
    for (size_t i = 0; i < input.size();) {
        if (input[i] == '$') {
            if (i + 1 < input.size() && input[i + 1] == '{') {
                // Handle ${VAR}
                size_t j = i + 2;
                while (j < input.size() && input[j] != '}') {
                    j++;
                }
                if (j < input.size()) {
                    std::string var_name = input.substr(i + 2, j - (i + 2));
                    auto it = user_env.find(var_name);
                    if (it != user_env.end()) {
                        result += it->second;
                    } else {
                        const char *env_val = std::getenv(var_name.c_str());
                        if (env_val)
                            result += env_val;
                    }
                    i = j + 1;
                    continue;
                } else {
                    // Unterminated ${
                    result += input[i++];
                    continue;
                }
            } else if (i + 1 < input.size() &&
                       (std::isalnum(input[i + 1]) || input[i + 1] == '_')) {
                // Handle $VAR
                size_t j = i + 1;
                while (j < input.size() &&
                       (std::isalnum(input[j]) || input[j] == '_')) {
                    j++;
                }
                std::string var_name = input.substr(i + 1, j - i - 1);
                auto it = user_env.find(var_name);
                if (it != user_env.end()) {
                    result += it->second;
                } else {
                    const char *env_val = std::getenv(var_name.c_str());
                    if (env_val)
                        result += env_val;
                }
                i = j;
                continue;
            }
        }
        result += input[i++];
    }
    return result;
}

bool process_variable_assignment(const std::string &input) {
    size_t eq_pos = input.find('=');
    if (eq_pos != std::string::npos && eq_pos > 0 && std::isalpha(input[0])) {
        std::string key = input.substr(0, eq_pos);
        std::string value = input.substr(eq_pos + 1);
        user_env[key] = value;
        return true;
    }
    return false;
}
