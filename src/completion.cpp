#include "completion.hpp"

#include <algorithm>  
#include <cstdlib>    
#include <cstring>    
#include <filesystem> 
#include <iostream>           
#include <readline/history.h> 
#include <readline/readline.h> 
#include <sstream> 
#include <string>
#include <unistd.h>      
#include <unordered_set> 
#include <vector>


std::string last_prefix;
bool last_multiple_matches = false;
int tab_press_count = 0;
std::vector<std::string> last_matches;

char *command_generator(const char *text, int state) {
    static size_t match_index;

    if (state == 0) {
        last_matches.clear();
        match_index = 0;

        std::string textStr(text);
        std::unordered_set<std::string> seen;

        const std::vector<std::string> vocabulary{"echo", "exit", "type",
                                                  "pwd",  "cd",   "history"};

        for (const auto &word : vocabulary) {
            if (word.compare(0, textStr.size(), textStr) == 0 &&
                word != textStr) {
                last_matches.push_back(word);
                seen.insert(word);
            }
        }

        const char *path_env = std::getenv("PATH");
        if (path_env) {
            std::istringstream iss(path_env);
            std::string dir;
            while (std::getline(iss, dir, ':')) {
                try {
                    if (!std::filesystem::exists(dir) ||
                        !std::filesystem::is_directory(dir))
                        continue;
                    for (const auto &entry :
                         std::filesystem::directory_iterator(dir)) {
                        std::string filename =
                        entry.path().filename().string(); if
                        (filename.compare(0, textStr.size(), textStr) == 0 &&
                            filename != textStr &&
                            access(entry.path().c_str(), X_OK) == 0 &&
                            seen.find(filename) == seen.end()) {
                            last_matches.push_back(filename);
                            seen.insert(filename);
                        }
                    }
                } catch (...) {
                    continue;
                }
            }
        }

        last_matches.erase(
            std::remove(last_matches.begin(), last_matches.end(), textStr),
            last_matches.end());

        std::sort(last_matches.begin(), last_matches.end());
    }

    if (match_index >= last_matches.size()) {
        return nullptr;
    } else {
        return strdup(last_matches[match_index++].c_str());
    }
}

char **custom_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;

    std::string current_prefix(text);

    if (current_prefix != last_prefix) {
        last_prefix = current_prefix;
        tab_press_count = 0;
        last_multiple_matches = false;
    }

    char **matches = rl_completion_matches(text, command_generator);

    int match_count = 0;
    if (matches) {
        while (matches[match_count] != nullptr)
            match_count++;
    }

    if (match_count > 1) {
        tab_press_count++;
        last_multiple_matches = true;

        if (tab_press_count == 1) {
            // ring the bell only, no printing
            std::cout << "\a" << std::flush;
            return matches;
        } else if (tab_press_count == 2) {
            std::cout << std::endl;

            // Skip the prefix itself if it's included as a match
            int start_index = 0;
            if (match_count > 0 && strcmp(matches[0], text) == 0) {
                start_index = 1;
            }

            for (int i = start_index; i < match_count; i++) {
                std::cout << matches[i] << "  ";
            }
            std::cout << std::endl;

            rl_on_new_line();
            rl_replace_line(rl_line_buffer, 0);
            rl_redisplay();
        }
    } else {
        tab_press_count = 0;
        last_multiple_matches = false;
    }

    return matches;
}
