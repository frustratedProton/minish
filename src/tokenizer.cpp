#include "tokenizer.hpp"

#include <iostream>
#include <vector>



std::vector<std::string> split(const std::string &input) {
    std::vector<std::string> tokens;
    std::string token;
    enum State { Unquoted, InSingleQuote, InDoubleQuote } state = Unquoted;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];

        if (state == Unquoted) {
            if (c == '\'') {
                state = InSingleQuote;
            } else if (c == '"') {
                state = InDoubleQuote;
            } else if (std::isspace(c)) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else if (c == '\\') {
                if (i + 1 < input.size()) {
                    token += input[i + 1];
                    ++i;
                }
            } else {
                token += c;
            }
        } else if (state == InSingleQuote) {
            if (c == '\'') {
                state = Unquoted;
            } else {
                token += c;
            }
        } else if (state == InDoubleQuote) {
            if (c == '\\') {
                if (i + 1 < input.size()) {
                    char next = input[i + 1];
                    if (next == '\\' || next == '"' || next == '$' ||
                        next == '\n') {
                        token += next;
                        ++i;
                    } else {
                        token += c;
                    }
                } else {
                    token += c;
                }
            } else if (c == '"') {
                state = Unquoted;
            } else {
                token += c;
            }
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}
