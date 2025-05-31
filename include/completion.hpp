#pragma once

#include <string>
#include <vector>

extern std::string last_prefix;
extern bool last_multiple_matches;
extern int tab_press_count;
extern std::vector<std::string> last_matches;

char *command_generator(const char *text, int state);
char **custom_completion(const char *text, int start, int end);