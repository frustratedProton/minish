#include "exec_pipes.hpp"

#include <string>
#include <iostream>
#include <vector>

std::vector<std::string> split_pipeline(const std::string &input) {
    std::vector<std::string> parts;
    std::string token;
    bool in_single_quote = false, in_double_quote = false;

    for (char c : input) {
        if (c == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
        } else if (c == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
        } else if (c == '|' && !in_single_quote && !in_double_quote) {
            parts.push_back(token);
            token.clear();
            continue;
        }
        token += c;
    }

    if (!token.empty())
        parts.push_back(token);

    return parts;
}
