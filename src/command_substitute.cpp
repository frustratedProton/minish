#include "command_substitute.hpp"

#include <string>
#include <algorithm>
#include <cstdio>
#include <string>

std::string substitute_command(const std::string &input) {
    std::string result;
    size_t pos = 0;

    while (pos < input.size()) {
        if (input[pos] == '$' && pos + 1 < input.size() &&
            input[pos + 1] == '(') {
            size_t start = pos + 2;
            int depth = 1;
            size_t end = start;

            while (end < input.size() && depth > 0) {
                if (input[end] == '(')
                    depth++;
                else if (input[end] == ')')
                    depth--;
                end++;
            }

            if (depth == 0) {
                std::string inner_cmd = input.substr(start, end - start - 1);
                std::string output;

                FILE *fp = popen(inner_cmd.c_str(), "r");
                if (fp) {
                    char buffer[128];
                    while (fgets(buffer, sizeof(buffer), fp)) {
                        output += buffer;
                    }
                    pclose(fp);

                    output.erase(
                        std::remove(output.begin(), output.end(), '\n'),
                        output.end());
                }

                result += output;
                pos = end;
                continue;
            }
        }

        result += input[pos++];
    }

    return result;
}