#pragma once
#include <string>
#include <vector>
std::string	to_lower(const std::string s);
std::string	trim(const std::string s);
std::string int_to_string(int nb);
bool	endsWith(const std::string& base, const std::string& substring);
std::vector<char *> to_char_str_tab(std::vector<std::string> &input);