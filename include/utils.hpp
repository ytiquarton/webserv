#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <ostream>

std::string return_path_routed(std::string path, ServerConfig& server);


#include <sys/stat.h>
#include <unistd.h>

bool existDir(const std::string& path);

#include "parse_config.hpp"

#endif
